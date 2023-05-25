require("tidyverse")

#関数宣言
# id: http://www1.river.go.jp/ で調べると出てくる水位計測地点の観測所記号
# from_year, from_month　水位を調べる年月
# to_year, to_month　数か月以上の期間を調べる場合は、終了年月
# interval 関数の待ち時間　サーバーの過負荷を避けるために0.2秒程度推奨
download.river_level = function(id,from_year,from_month,to_year=NULL,to_month=NULL,interval=0.2,output=TRUE){
	if(is.null(to_year) || is.null(to_month)){
		if(!is.null(to_year)){
			warning("to_year is ignored because to_month is NULL.")
		}
		if(!is.null(to_month)){
			warning("to_month is ignored because to_year is NULL.")
		}
		url = sprintf(
			"http://www1.river.go.jp/cgi-bin/DspWaterData.exe?KIND=2&ID=%s&BGNDATE=%04d%02d01&ENDDATE=%04d%02d01&KAWABOU=NO",
			id,from_year,from_month,from_year+1,from_month
		)
		
		if(output)cat(paste0("loading ",from_year,".",from_month,"@",id,"\n"))
		Sys.sleep(interval)
		html = rvest::read_html(url)
		
		if(str_detect(html%>%rvest::html_text(),"パラメータに誤りがあります")){
			stop(paste0("fail to find data with id \"",id,"\""))
		}
		
		nodes = html %>% rvest::html_nodes(xpath = "//center/table")
		table = nodes[2] %>% rvest::html_table() %>% 
			.[[1]] %>%
			.[-1,]
		
		table[1,1] = "date"
		names(table) = as.character(table[1,])
		table = table[-1,]
		data = table %>%
			mutate(date=as.Date(date)) %>%
			filter(!is.na(date)) %>%
			pivot_longer(names_to = "hour",values_to="level",cols=2:ncol(.)) %>%
			mutate(level=as.numeric(level) %>% suppressWarnings()) %>%
			mutate(datetime = as_datetime(paste0(strftime(date,"%Y-%m-%d "),sprintf("%s:00:00 JST",str_extract(hour,"[0-9]+")))),.before=date) %>%
			select(datetime,level)
	}else{
		mbegin = from_year*12 + (from_month-1)
		mend = to_year*12 + (to_month-1)
		if(mbegin>mend){
			mtmp = mbegin
			mbegin=mend
			mend = mtmp
		}
		
		data = NULL
		for(mno in mbegin:mend){
			year = as.integer(mno/12)
			month = mno%%12 + 1
			data = data %>% 
				bind_rows(download.river_level(id,year,month,interval=interval))
		}
	}

	return(data)
}

#雁来の雪解けが落ち着くのはいつ？
#id = "301031281101520"
id = "301031281101170"
#10年間のベースデータ入手
basedata = download.river_level(id,2013,1,2022,12)
base = basedata %>%
	mutate(day = strftime(datetime,"%j")%>%as.integer()) %>%
	group_by(day) %>% summarise(
		min = min(level,na.rm=TRUE),
		q25 = quantile(level,0.25,na.rm=TRUE),
		med = median(level,na.rm=TRUE),
		q75 = quantile(level,0.75,na.rm=TRUE),
		max = max(level,na.rm=TRUE),
		.groups = "drop"
	) %>%
	mutate(datetime = as_datetime("2023-01-01 0:00:00 JST") + ddays(day-1))
#今年のデータを入手
data = download.river_level(id,2023,1,2023,5)


#水位を表示
ggplot() +
	geom_ribbon(data=base,aes(x=datetime,ymin=min,ymax=max),alpha=0.1) +
	geom_ribbon(data=base,aes(x=datetime,ymin=q25,ymax=q75),alpha=0.25) +
	geom_line(data=base,aes(x=datetime,y=med),alpha=0.5) +
	geom_line(data=data,aes(x=datetime,y=level),color="red")+
	#xlim(as_datetime("2023-01-01 00:00:00"), as_datetime("2023-07-01 00:00:00")) + 
	NULL





#石狩川比較
data_points = data.frame(name="石狩河口", id = "301031281101120") %>%
	bind_rows(data.frame(name="石狩大橋", id = "301031281101100")) %>%
	bind_rows(data.frame(name="岩見沢大橋", id = "301031281101090")) %>%
	bind_rows(data.frame(name="月形", id="301031281101080")) %>%
	bind_rows(data.frame(name="砂川橋", id = "301031281101060")) %>%
	return()

data = tibble()
for(i in 1:nrow(data_points)){
	data_point = data_points[i,]
	
	#対象地点の水位データを2023年1月から2023年5月まで取得
	level = download.river_level(data_point$id,2023,1,2023,5)%>%
		mutate(location = data_point$name,.after=datetime)
	data = data %>%
		bind_rows(level)
}

#水位を表示
data %>%
	ggplot() +
	geom_line(aes(x=datetime,y=level,color=location,group=location))+
	NULL

#各地点の最低水位計算
mindata = data %>% group_by(location) %>% summarise(level_min=min(level,na.rm=TRUE))

#最低水位との差分を表示
data %>%
	left_join(mindata,by="location") %>% 
	mutate(level_dif = level - level_min) %>%
	ggplot() +
	geom_line(aes(x=datetime,y=level_dif,color=location,group=location))+
	NULL
