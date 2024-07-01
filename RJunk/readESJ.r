require("tidyverse")
htmlat = hmRLib::file.at("https://esj.ne.jp/meeting/abst/71")
url = sprintf(htmlat("oral_index.html"))
top.html = rvest::read_html(url)

html.ul = top.html |>
	rvest::html_nodes(xpath = '//div[@class="session_list_area"]//li')

talk.table = NULL
for(slot.i in 1:length(html.ul)){
	text = html.ul[slot.i]|>rvest::html_text2()
	aref = html.ul[slot.i]|>rvest::html_nodes(xpath = "a")
	
	sloturl = aref[2] |> rvest::html_attr("href")
	slot = aref[2]|> rvest::html_text2() |> stringr::str_remove(r"(\s\-$)")
	datetime = text |> stringr::str_remove(sprintf("\\s*%s.*$",slot|>hmRLib::escape_regex()))
	date = datetime |> stringr::str_remove("\\s+.*$")
	slottime = datetime |> stringr::str_remove("^.+\\s+")
	slottime.start = slottime |> stringr::str_remove("\\-.*$")
	
	html = rvest::read_html(htmlat(sloturl))
	pset = html |> rvest::html_nodes(xpath = 'body/p')
	
	roomtext = 	html |> 
		rvest::html_nodes(xpath = '//div[@class="index_page_header"]/h4') |>
		rvest::html_text2()
	roomtext = roomtext[2]

	session = roomtext |> stringr::str_remove("\\s*\\(.+\\)")
	room = roomtext |> 
		stringr::str_extract("\\s*\\((.+)\\)",1) |> 
		stringr::str_remove(sprintf("%s\\s*",datetime|>hmRLib::escape_regex()))

	for(talk.i in 1:length(pset)){
#		pelems = pset[talk.i] |> rvest::html_children()
		title = pset[talk.i] |> rvest::html_nodes(xpath="strong") |> rvest::html_text2()
		author = pset[talk.i] |> rvest::html_nodes(xpath="span") |> rvest::html_text2()
		
		if(length(title)==0)next
		talkaref = pset[talk.i]  |> rvest::html_nodes("a")
		talkurl = talkaref[2] |> rvest::html_attr("href")
		talkid = talkaref[2] |> rvest::html_text2()
		
		#talkno = talkid|>stringr::str_extract("[0-9]+$")|>as.integer()
		talk.time = lubridate::make_datetime(
			hour=slottime.start|>stringr::str_remove(":.+$")|>as.integer(),
			min =slottime.start|>stringr::str_remove("^.+:")|>as.integer()) + lubridate::minutes(15*(talk.i-1))
			
		if(length(title)>1){
			title.en = title[2]
			title = title[1]
		}else{
			title = title
			title.en = title
		}

		if(length(author)>1){
			author.en = author[2]
			author = author[1]
		}else{
			author.en = author
			author = author
		}
		duration =15
		talk.time.end = talk.time + minutes(duration)
		talk.table = rbind(talk.table,data.frame(
			date = date,
			time.beg = sprintf("%02d:%02d",hour(talk.time),minute(talk.time)),
			time.end = sprintf("%02d:%02d",hour(talk.time.end),minute(talk.time.end)),
			timemin = 60*hour(talk.time)+minute(talk.time),duration=duration,
			slot=slot, session = session, room=room,
			title = title,title.en=title.en,
			author=author,author.en=author.en, 
			url = talkurl,id = talkid
		))
	}	
}


for(date.f in unique(talk.table$date)){
	data = talk.table |> filter(date==date.f)
	rooms = unique(data$room)
	times = c(data$time.beg,data$time.end) |> unique() |>sort()
	
	fout = hmRLib::file.fout(sprintf("%s.md",date.f))
	
	fout("<table>")
	fout("<colgroup>")
	for(i in 0:length(rooms)){
		fout("\t<col />")
	}
	fout("</colgroup>")
	
	fout('<thead><tr class="header">')
	fout('<th>time</th>')
	for(room in rooms){
		fout('<th>%s</th>',room)
	}
	fout('</tr></thead>')
	
	rtype = "odd"
	fout('<tbody>')
	for(time.f in times[1:(length(times)-1)]){
		fout('<tr class="%s"><th>%s - </th>',rtype,time.f)
		#rtype = ifelse(rtype=="even","odd","even")
		
		for(room.f in rooms){
			datum = data |> filter(time.beg==time.f & room==room.f)
			prevd = data |> filter(time.end==time.f & room==room.f)
			if(nrow(datum)){
				if(nrow(datum)>1)warning(sprintf("%s %s %s %d",date.f,time.f,room.f,nrow(datum)))
				cellnum = which(times == datum$time.end) - which(times == datum$time.beg)

				fout("<td rowspan=%d> <a href=%s>%s</a><br/>%s</td>",
					  cellnum,htmlat(datum$url),datum$title,datum$author|>stringr::str_remove_all("（.+）"))

			}else if(time.f==head(times,1) || nrow(prevd)){
				nextd = data |> filter(time.beg>time.f & room==room.f) |> head(1)
				
				if(nrow(nextd)){
					cellnum = which(times==nextd$time.beg) - which(times == time.f)
				}else{
					cellnum = length(times)- which(times == time.f)
				}

				fout("<td rowspan=%d></td>",cellnum)

			}
		}
		fout('</tr>')
	}
	fout('</tbody>')
	
	fout('</table>')
	
	fout(close=TRUE)
}
