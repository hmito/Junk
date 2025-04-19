install.packages("urltools")
urltools::url_encode("登米・東部栗原")
"門脇"
"和渕"
"剣先"



"落堀"
"若石大橋"

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
MIRAI.detail.read_html = function(Gamen,date,hour,is_detailed,pageGroup,stationNo){
	url = paste0(
		"https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/",
		Gamen,
		"?param=common=",
		"dispDate:",as.character(date),"-",sprintf("%02d",hour),"-50",
		"$duration:",ifelse(is_detailed,"10","60"),
		"$pageGroup:",urltools::url_encode(pageGroup),
		"$stationNo:",stationNo
	)
	return(rvest::read_html(url,encoding = "shift_jis"))
}
MIRAI.wlevel.find_station = function(station_name,station_lists){
	return(
		station_lists |>
			dplyr::filter(種別=="水位" & 観測局名==station_name) 
	)
}
MIRAI.wlevel.read_station = function(html){
	station = html |>
		rvest::html_elements(xpath = '//div[@id="MsgStation"]')|>
		rvest::html_text2() |>
		stringr::str_remove_all("[\\r\\s]")
	return(station)
}
MIRAI.wlevel.read_zerolevel = function(html){
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
MIRAI.wlevel.read_table = function(html){
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
	
	data = data.frame(time = lubridate::ymd_hms(data[,1]), value = as.numeric(data[,2]))
	
	return(list(info=info,data=data))
}

urltools::url_decode("%E3%80%80")




MIRAI.read_wlevel(station_name,from,to=NULL,station_lists=NULL){
	if(is.null(to)){
		to = lubridate::now("Japan")
	}
	if(is.null(station_lists)){
		station_lists = MIRAI.load_station_list()
	}
	station = MIRAI.wlevel.find_station(station_name,station_lists)
	station$page
}

station_name = "若石大橋"
Gamen = "Gamen22Servlet"
date = as.Date("2025-03-31")
hour = 23
is_detailed = TRUE
pageGroup = "仙台土木事務所"
stationNo = "104004083"

MIRAI.wlevel.read_station(html)
MIRAI.wlevel.read_zerolevel(html)
MIRAI.wlevel.read_table(html)

html = MIRAI.read_html(Gamen,date,hour,is_detailed,pageGroup,stationNo)
MIRAI.detail.get_commonParam(html)
Stations = MIRAI.load_station_list()

MIRAI.read_water_level = ()
https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/Gamen22Servlet?param=common=dispDate:2025-04-19-14-00$duration:60$timeNumber:24$pageGroup:%E8%A5%BF%E9%83%A8%E4%BB%99%E5%8F%B0
commonParam = MIRAI.detail.get_commonParam(html)
cm = as.list(commonParam[,2])
names(cm) = commonParam[,1]
url = "https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/Gamen22Servlet?param=common=dispDate:2025-04-18-09-00$duration:60$timeNumber:24$pageGroup:%E5%8C%97%E4%B8%8A%E5%B7%9D%E4%B8%8B%E6%B5%81%E6%B2%B3%E5%B7%9D%E4%BA%8B%E5%8B%99%E6%89%80$page:1$itemPageGroup:1H$itemPage:1$stationNo:104004290$kioDispDate:null$kioPattern:1$dispEventNumber:1$station2No:0$autoType:false$riverName:null$mapClassSwich:0$lScaleMax:0$lScaleMin:0$rScaleMax:0$rScaleMin:0$lScaleMax2:0$lScaleMin2:0$rScaleMax2:0$rScaleMin2:0$ShiftWidth:0$anypage:0$freeStr:null$freeNo:1"
html = rvest::read_html(url,encoding = "shift_jis")


url = 	"https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/Gamen22Servlet?param=station=27?common=dispDate:2025-04-16-23-00$duration:60$timeNumber:24$pageGroup:%E7%99%BB%E7%B1%B3%E3%83%BB%E6%9D%B1%E9%83%A8%E6%A0%97%E5%8E%9F$page:14$itemPageGroup:1H$itemPage:1$stationNo:104004002$kioDispDate:null$kioPattern:1$dispEventNumber:1$station2No:0$autoType:true$riverName:null$mapClassSwich:0$lScaleMax:0$lScaleMin:0$rScaleMax:0$rScaleMin:0$lScaleMax2:0$lScaleMin2:0$rScaleMax2:0$rScaleMin2:0$ShiftWidth:0$anypage:0$freeStr:null$freeNo:1"
url = 	"https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/Gamen22Servlet?param=station=29?common=dispDate:2025-04-01-23-00$duration:60$timeNumber:24$pageGroup:%E7%99%BB%E7%B1%B3%E3%83%BB%E6%9D%B1%E9%83%A8%E6%A0%97%E5%8E%9F$page:10$itemPageGroup:1H$itemPage:1$stationNo:104004001$kioDispDate:null$kioPattern:1$dispEventNumber:1$station2No:0$autoType:true$riverName:null$mapClassSwich:0$lScaleMax:0$lScaleMin:0$rScaleMax:0$rScaleMin:0$lScaleMax2:0$lScaleMin2:0$rScaleMax2:0$rScaleMin2:0$ShiftWidth:0$anypage:0$freeStr:null$freeNo:1"

from = as.Date("2025-04-10")
to = as.Date("2025-04-17")
?stringr::str_conv
miyagi
read_water_level_on_dobokusougou_pref_miyagi = function(url, from, to,zero_level=NULL){
	html = rvest::read_html(url,encoding = "shift_jis")
	
	if(to - from > 20)stop("too long duration")

	station = html |>
		rvest::html_elements(xpath = '//div[@id="MsgStation"]')|>
		rvest::html_text2() |>
		stringr::str_remove_all("[\\r\\s]")
	
	if(is.null(zero_level)){
		zero_info = html |>
			rvest::html_elements(xpath = '//div[@id="MsgZero"]')|>
			rvest::html_text2() |>
			stringr::str_remove_all("[\\r\\s]") |>
			stringr::str_extract("([KT]P)([+\\-]?[0-9\\.]+)",c(1,2))
		
		if(zero_info[1]=="KP"){
			zero_level =as.numeric(zero_info[2]) -0.8745
		}else{
			zero_level = as.numeric(zero_info[2])
		}
		
	}
	
	url.format = url |> 
		stringr::str_replace("[0-9]{4}-[0-9]{2}-[0-9]{2}-[0-9]{2}-[0-9]{2}","########-23-00") |>
		stringr::str_replace("duration:10","duration:60")
	
	
	levels = numeric(0)
	day = from
	while(day < to){
		Sys.sleep(1)
		path = url.format|>stringr::str_replace("########",as.character(day))
		print(path)
		html = rvest::read_html(path,encoding = "shift_jis")
	
		station = html |>
			rvest::html_elements(xpath = '//div[@id="MsgStation"]')|>
			rvest::html_text2() |>
			stringr::str_remove_all("[\\r\\s]")
		
		zero_heigth = html |>
			rvest::html_elements(xpath = '//div[@id="MsgZero"]')|>
			rvest::html_text2() |>
			stringr::str_remove_all("[\\r\\s]")
		
		data = html |>
			rvest::html_elements(xpath = '//table[@id="Listblock_table"]')|>
			rvest::html_table()
		data = data[[1]] |>
			as.data.frame()
		
		
		level.this = data[12:35,2] |> stringr::str_remove("[\\r\\n]+.*?$")|> stringr::str_remove("[\\r\\n]+.*?$") |>as.numeric()
		levels = c(levels,level.this)
		day = day + 1
	}
	return(list(station = station,level = levels+zero_level, raw_level = levels, zero_level=zero_level))
}



Numaguchi = read_water_level_on_dobokusougou_pref_miyagi(
	"https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/Gamen22Servlet?param=station=27?common=dispDate:2025-04-16-23-00$duration:60$timeNumber:24$pageGroup:%E7%99%BB%E7%B1%B3%E3%83%BB%E6%9D%B1%E9%83%A8%E6%A0%97%E5%8E%9F$page:14$itemPageGroup:1H$itemPage:1$stationNo:104004002$kioDispDate:null$kioPattern:1$dispEventNumber:1$station2No:0$autoType:true$riverName:null$mapClassSwich:0$lScaleMax:0$lScaleMin:0$rScaleMax:0$rScaleMin:0$lScaleMax2:0$lScaleMin2:0$rScaleMax2:0$rScaleMin2:0$ShiftWidth:0$anypage:0$freeStr:null$freeNo:1",
	as.Date("2025-04-10"),
	as.Date("2025-04-17")
)

Arakawa =  read_water_level_on_dobokusougou_pref_miyagi(
	"https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/Gamen22Servlet?param=station=29?common=dispDate:2025-04-01-23-00$duration:60$timeNumber:24$pageGroup:%E7%99%BB%E7%B1%B3%E3%83%BB%E6%9D%B1%E9%83%A8%E6%A0%97%E5%8E%9F$page:10$itemPageGroup:1H$itemPage:1$stationNo:104004001$kioDispDate:null$kioPattern:1$dispEventNumber:1$station2No:0$autoType:true$riverName:null$mapClassSwich:0$lScaleMax:0$lScaleMin:0$rScaleMax:0$rScaleMin:0$lScaleMax2:0$lScaleMin2:0$rScaleMax2:0$rScaleMin2:0$ShiftWidth:0$anypage:0$freeStr:null$freeNo:1",
	as.Date("2025-04-10"),
	as.Date("2025-04-17"),
	4.21
)

Sanuma = read_water_level_on_dobokusougou_pref_miyagi(
	"https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/Gamen22Servlet?param=station=18?common=dispDate:2025-04-01-23-00$duration:60$timeNumber:24$pageGroup:%E7%99%BB%E7%B1%B3%E3%83%BB%E6%9D%B1%E9%83%A8%E6%A0%97%E5%8E%9F$page:30$itemPageGroup:1H$itemPage:1$stationNo:104004099$kioDispDate:null$kioPattern:1$dispEventNumber:1$station2No:0$autoType:true$riverName:null$mapClassSwich:0$lScaleMax:0$lScaleMin:0$rScaleMax:0$rScaleMin:0$lScaleMax2:0$lScaleMin2:0$rScaleMax2:0$rScaleMin2:0$ShiftWidth:0$anypage:0$freeStr:null$freeNo:1",
	as.Date("2025-04-10"),
	as.Date("2025-04-17")
)

Ochihori = read_water_level_on_dobokusougou_pref_miyagi(
	"https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/Gamen22Servlet?param=common=dispDate:2025-04-15-14-00$duration:60$timeNumber:24$pageGroup:%E7%99%BB%E7%B1%B3%E3%83%BB%E6%9D%B1%E9%83%A8%E6%A0%97%E5%8E%9F$page:3$itemPageGroup:1H$itemPage:1$stationNo:104004098$kioDispDate:null$kioPattern:1$dispEventNumber:1$station2No:0$autoType:true$riverName:null$mapClassSwich:0$lScaleMax:0$lScaleMin:0$rScaleMax:0$rScaleMin:0$lScaleMax2:0$lScaleMin2:0$rScaleMax2:0$rScaleMin2:0$ShiftWidth:0$anypage:0$freeStr:null$freeNo:1",
	as.Date("2025-04-10"),
	as.Date("2025-04-17")
)

Mikatajima = read_water_level_on_dobokusougou_pref_miyagi(
	"https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/Gamen22Servlet?param=station=17?common=dispDate:2025-04-15-14-00$duration:60$timeNumber:24$pageGroup:%E7%99%BB%E7%B1%B3%E3%83%BB%E6%9D%B1%E9%83%A8%E6%A0%97%E5%8E%9F$page:36$itemPageGroup:1H$itemPage:1$stationNo:104004267$kioDispDate:null$kioPattern:1$dispEventNumber:1$station2No:0$autoType:true$riverName:null$mapClassSwich:0$lScaleMax:0$lScaleMin:0$rScaleMax:0$rScaleMin:0$lScaleMax2:0$lScaleMin2:0$rScaleMax2:0$rScaleMin2:0$ShiftWidth:0$anypage:0$freeStr:null$freeNo:1",
	as.Date("2025-04-10"),
	as.Date("2025-04-17")
)


plot(Numaguchi$level,ylim=c(3,8),type="l")
points(1:168,Sanuma$level,col="red",type="l")
points(1:168,Ochihori$level,col="blue",type="l")
points(1:168,Arakawa$raw_level,col="forestgreen",type="l")
points(1:168,Mikatajima$level,col="gold",type="l")

target.format = "https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/Gamen22Servlet?param=station=%d?common=dispDate:%s-23-00$duration:60$timeNumber:24$pageGroup:%E7%99%BB%E7%B1%B3%E3%83%BB%E6%9D%B1%E9%83%A8%E6%A0%97%E5%8E%9F"


"沼口"
"荒川Ｓ(下)"
"佐沼"

stnos = c(19,28,30)

path = sprintf(target.format,22,as.date(from))
path = "https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/Gamen22Servlet?param=timeDown=24?common=dispDate:2025-04-14-22-00$duration:10$timeNumber:24$pageGroup:%E8%A5%BF%E9%83%A8%E6%A0%97%E5%8E%9F$page:1$itemPageGroup:1H$itemPage:1$stationNo:104004011$kioDispDate:null$kioPattern:1$dispEventNumber:1$station2No:0$autoType:true$riverName:null$mapClassSwich:0$lScaleMax:0$lScaleMin:0$rScaleMax:0$rScaleMin:0$lScaleMax2:0$lScaleMin2:0$rScaleMax2:0$rScaleMin2:0$ShiftWidth:0$anypage:0$freeStr:null$freeNo:1"



path = "https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/Gamen22Servlet?param=station=29?common=dispDate:2025-04-16-23-00$duration:60$timeNumber:24$pageGroup:%E7%99%BB%E7%B1%B3%E3%83%BB%E6%9D%B1%E9%83%A8%E6%A0%97"


#table id="Listblock_table"


html |>
#div id="MsgStation"


	


date = as.Date("2025-04-15")

datestr = format(date)
path = "https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/Gamen21Servlet?param=station=%d?common=dispDate:%s-09-00$duration:10"


"https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/Gamen95Servlet?param=0=0?common=dispDate:2025-04-13-09-00$duration:60$timeNumber:24$pageGroup:%E7%99%BB%E7%B1%B3%E3%83%BB%E6%9D%B1%E9%83%A8%E6%A0%97%E5%8E%9F$page:1$itemPageGroup:1H$itemPage:1$stationNo:104004201$kioDispDate:null$kioPattern:1$dispEventNumber:1$station2No:0$autoType:false$riverName:null$mapClassSwich:0$lScaleMax:0$lScaleMin:0$rScaleMax:0$rScaleMin:0$lScaleMax2:0$lScaleMin2:0$rScaleMax2:0$rScaleMin2:0$ShiftWidth:0$anypage:0$freeStr:null$freeNo:1"

"https://www.dobokusougou.pref.miyagi.jp/miyagi/servlet/Gamen21Servlet?param=common=dispDate:2025-03-17-21-50$duration:10$timeNumber:24$pageGroup:%E5%A4%A7%E5%B4%8E%E5%9C%B0%E6%96%B9%E3%83%80%E3%83%A0%E7%B7%8F%E5%90%88$page:1$itemPageGroup:1H$itemPage:1$stationNo:104001064$kioDispDate:null$kioPattern:1$dispEventNumber:1$station2No:0$autoType:true$riverName:null$mapClassSwich:0$lScaleMax:0$lScaleMin:0$rScaleMax:0$rScaleMin:0$lScaleMax2:0$lScaleMin2:0$rScaleMax2:0$rScaleMin2:0$ShiftWidth:0$anypage:0$freeStr:null$freeNo:1?"