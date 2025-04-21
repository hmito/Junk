# MIyagi River And basin Information system (MIRAI)
# https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/Gamen1Servlet

MIRAI.load_station_list = function(URL = "https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/Gamen95Servlet?param=0=0",encoding = "shift_jis"){
	html = rvest::read_html(URL,encoding=encoding)
	
	
	elems = html |> 
		rvest::html_elements(xpath='//span[@class="nam"]')
	
	data = NULL
	for(elem in elems){
		station = elem	|>
			rvest::html_text2()
		onclick = elem |>
			rvest::html_attr("onclick") |>
			stringr::str_remove_all("[\\n\\r]")|>
			stringr::str_extract("\\((.+)\\)",1) |>
			stringr::str_remove_all("'")|>
			stringr::str_split(",")
		gamen = onclick[[1]][1]
		stationid = onclick[[1]][3]
		page = onclick[[1]][5]
		
		data = rbind(data,data.frame(stationno=stationid,gamen=gamen,page=page))
	}
	
	table = html |> 
		rvest::html_element("#Listblock_table") |>
		rvest::html_table(header = TRUE)
	
	table =cbind(table,data)
	table = table[table["種別"]!="",]
	
	return(table)
}
MIRAI.detail.get_commonParam = function(html){
	ans = html |>
		rvest::html_elements(xpath = '//script')|>
		rvest::html_text2() |>
		paste(collapse="\r") |>
		stringr::str_extract('var[\\t\\s]*commonParam[\\t\\s]*=[\\t\\s]*"([^"]+)"',group = 1) |>
		stringr::str_split("\\$")
	
	commonParam = ans[[1]] |>stringr::str_split(":",simplify = TRUE)
	cm = as.list(commonParam[,2])
	names(cm) = commonParam[,1]
	
	return(cm)
}
MIRAI.detail.set_commonParam = function(Gamen,param, commonParam){
	return(
		paste0("https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/",
				 Gamenm,
				 "?param=",
				 param,
				 "?common=",
				 paste(apply(data.frame(x=names(commonParam),y=as.character(commonParam)),1,function(x){paste(x,collapse=":")}),collapse="$")
		)
	)
}
MIRAI.detail.read_html = function(Gamen, pageGroup, stationNo, timepoint, detailed){
	if(detailed){
		duration = 60
	}else{
		duration = 10
	}
	url = paste0(
		"https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/",
		Gamen,
		"?param=common=",
		"dispDate:",format(timepoint,"%Y-%m-%d-%H-%M"),
		"$duration:",duration,
		"$pageGroup:",urltools::url_encode(pageGroup),
		"$stationNo:",stationNo
	)
	return(rvest::read_html(url,encoding = "shift_jis"))
}
MIRAI.detail.wlevel.find_station = function(station_name,station_lists){
	stations = station_lists |> 
		dplyr::mutate(name = 観測局名 |> 
						  	stringr::str_replace_all("（","\\(") |> 
						  	stringr::str_replace_all("）","\\)") |>
						  	stringr::str_remove_all("[\\s　]") ) |>
		dplyr::select(type = 種別, name,stationno,gamen,page)
	
	stations = stations |> 
		dplyr::filter(type=="水位")

	requires = data.frame(name = station_name |> 
								 	stringr::str_replace_all("（","\\(") |> 
								 	stringr::str_replace_all("）","\\)") |>
								 	stringr::str_remove_all("[\\s　]") )
	
	return(
		requires |> 
			dplyr::left_join(stations,by="name") 
	)
}
MIRAI.detail.wlevel.read_station = function(html){
	station = html |>
		rvest::html_elements(xpath = '//div[@id="MsgStation"]')|>
		rvest::html_text2() |>
		stringr::str_remove_all("[\\r\\s]")
	return(station)
}
MIRAI.detail.wlevel.read_zerolevel = function(html){
	zero_info = html |>
		rvest::html_elements(xpath = '//div[@id="MsgZero"]')|>
		rvest::html_text2() |>
		stringr::str_remove_all("[\\r\\s]") |>
		stringr::str_extract("([KT]P)([+\\-]?[0-9\\.]+)",c(1,2))
	
	if(any(is.na(zero_info))){
		return(list(zero_level = NA,zero_level.type=NA))
	}
	
	if(zero_info[1]=="KP"){
		zero_level =as.numeric(zero_info[2]) -0.8745
	}else{
		zero_level = as.numeric(zero_info[2])
	}
	return(list(zero_level = zero_level,zero_level.type=zero_info[1]))
}
MIRAI.detail.wlevel.read_table = function(html){
	data = html |>
		rvest::html_elements(xpath = '//table[@id="Listblock_table"]')|>
		rvest::html_table()
	data = data[[1]] |>
		as.data.frame() |>
		apply(c(1,2),function(x){x|>stringr::str_remove_all("[\\r\\n\\t]")|>stringr::str_remove("\\s{10,}.*$")})
	
	
	data_start = which(stringr::str_detect(data[,1],"月／日"))[1]
	
	info = as.list(data[1:(data_start-1),2])
	names(info) = data[1:(data_start-1),1]
	
	data = data[(data_start+1):nrow(data),]
	year = Sys.Date()  |>lubridate::year()
	month = Sys.Date()  |>lubridate::month()
	
	months =data[,1]|>stringr::str_sub(1,2) |> as.numeric()
	
	colnames(data) = c("datestr","valuestr")
	data[,1]=paste0(ifelse(month==1 & months==12,year-1,year),"/",data[,1],":00")
	
	data = data.frame(time = lubridate::ymd_hms(data[,1],tz="Japan"), value = as.numeric(data[,2]))
	
	return(list(info=info,data=data))
}
MIRAI.detail.timepoint_range = function(days = NULL, from = NULL, to = NULL, detailed=FALSE){
	now = lubridate::now("Japan")
	#dat.end = now  - lubridate::second(now) -  lubridate::minutes(as.integer(lubridate::minute(now))%%10)
	#dat.beg = dat.end - lubridate::days(30) + lubridate::minutes(10)
	if(is.null(to)){
		if(is.null(from)){
			if(is.null(days)){
				if(detailed){
					days = 3
				}else{
					days = 7
				}
			}
			to = lubridate::as_datetime(sprintf("%04d-%02d-%02d 23:50:00",lubridate::year(now),lubridate::month(now),lubridate::day(now)),tz="Japan")
			from = to - lubridate::days(days) + lubridate::minutes(10)
		}else{
			if(!lubridate::is.timepoint(from)){
				stop("argument \"from\" must be date or datetime.")
			}
			if(lubridate::is.Date(from)){
				from = lubridate::as_datetime(from,tz = "Japan")
			}
			to = lubridate::as_datetime(sprintf("%04d-%02d-%02d 23:50:00",lubridate::year(now),lubridate::month(now),lubridate::day(now)),tz="Japan")
		}
	}else{
		if(!lubridate::is.timepoint(to)){
			stop("argument \"to\" must be date or datetime.")
		}
		if(lubridate::is.Date(to)){
			to = to + lubridate::days(1) - lubridate::minutes(10)
		}
		if(is.null(from)){
			if(is.null(days)){
				if(detailed){
					days = 3
				}else{
					days = 7
				}
			}
			from = to - lubridate::days(days) + lubridate::minutes(10)
		}else{
			if(!lubridate::is.timepoint(from)){
				stop("argument \"from\" must be date or datetime.")
			}
			if(lubridate::is.Date(from)){
				from = lubridate::as_datetime(from,tz = "Japan")
			}
		}
	}
	if(!detailed){
		from = from + lubridate::minutes((60 - lubridate::minute(from))%%60)
		to = to - lubridate::minutes(lubridate::minute(to))
	}
	return(list(beg = from, end = to))
}
MIRAI.read_wlevel = function(station_name, days = NULL, from = NULL, to = NULL, detailed=FALSE, station_lists = NULL){
	if(is.null(station_lists)){
		station_lists = MIRAI.load_station_list()
	}
	station = MIRAI.detail.wlevel.find_station(station_name,station_lists)
	
	if(length(station)==0 || nrow(station)==0){
		stop(sprintf('fail to find station "%s"',station_name))
	}else if(nrow(station)>1){
		stop(sprintf('multiple stations named "%s" are found',station_name))
	}
	
	range = MIRAI.detail.timepoint_range(days,from,to,detailed)
#	range = MIRAI.detail.timepoint_range(14,NULL,NULL,FALSE)
	unitmin = ifelse(detailed,10,60)
	

	timepoint = range$end

	data = NULL

	for(wdt in (1:(4*35))){
		html = MIRAI.detail.read_html(station$gamen,station$page,station$stationno,timepoint,detailed)
		req.beg = timepoint - 23*lubridate::minutes(unitmin)
		req.end = timepoint
		
		if(is.null(data)){
			station_name = MIRAI.detail.wlevel.read_station(html) |> 
				stringr::str_replace_all("（","\\(") |> 
				stringr::str_replace_all("）","\\)") |>
				stringr::str_remove_all("[\\s　]")
			if(station_name!=station$name){
				warning(sprintf('fail to read station "%s"',station$name))
				break
			}
			zerolevel = MIRAI.detail.wlevel.read_zerolevel(html)
			if(is.na(zerolevel$zero_level)){
				warning(sprintf('fail to find zero-level information at station "%s"',station$name))
			}
			table = MIRAI.detail.wlevel.read_table(html)

			info = c(table$info,zerolevel)
		}else{
			table = MIRAI.detail.wlevel.read_table(html)
		}
		
		dat.beg = head(table$data$time,1)
		dat.end = tail(table$data$time,1)

		misstail = lubridate::time_length(req.end - dat.end,unit = "minutes")
		if(misstail>0){
			tailnum = lubridate::time_length(range$end - dat.end,unit = "minutes")/unitmin
			table$data = rbind(table$data,data.frame(time = dat.end+(1:tailnum)*lubridate::minutes(unitmin),value = NA_real_))
		}

		misshead = lubridate::time_length(dat.beg - req.beg,unit = "minutes")
		if(misshead>0){
			headnum = lubridate::time_length(dat.beg - range$beg,unit = "minutes")/unitmin
			table$data = rbind(data.frame(time = dat.beg-(headnum:1)*lubridate::minutes(unitmin),value = NA_real_),table$data)
			break
		}
		
		headmin = lubridate::time_length(range$beg - dat.beg,unit = "minutes")
		if(headmin >= 0){
			headrownum = headmin/unitmin
			table$data = table$data[-(1:headrownum),]
			break
		}
		
		data = rbind(table$data,data)
		timepoint = dat.beg - lubridate::minutes(unitmin)
	}
	
	return(list(data=data, info=info))
}

ans = MIRAI.read_wlevel("山内",10)
plot(ans$data$time,ans$data$value+ans$info$zero_level,type="l")

