if(!require(RMeCab)){
	install.packages("RMeCab", repos = "https://rmecab.jp/R", type = "source") 
}

require("tidyverse")

#タイトルとアブストをすべてファイルにいったん保存
if(0){
for(ESJno in 63:72){
	htmlat = hmRLib::file.at(sprintf("https://esj.ne.jp/meeting/abst/%d",ESJno))
	
	sessions = NULL
	target_url = c("oral_index.html","poster_index.html","sympo_index.html","jiyu_index.html")
	for(url in target_url){
		top.html = rvest::read_html(htmlat(url))
		session.elements = top.html |>
			rvest::html_elements(xpath = '//div[@class="session_list_area"]//a')
		
		sessions = rbind(sessions,
							  data.frame(
							  	name = session.elements|>
							  		rvest::html_text(),
							  	url = session.elements|>
							  		rvest::html_attr("href")
							  )
		)
	}
	sessions = sessions |> filter(!is.na(url) & (url |> stringr::str_detect("(html|htm)$")))
	
	talk_master = NULL
	for(session.url in sessions$url){
		session.html = rvest::read_html(htmlat(session.url))
		
		talk.elements = session.html |>
			rvest::html_elements(xpath = '//p//a')
		
		talks = data.frame(
			name = talk.elements|>
				rvest::html_text(),
			url = talk.elements|>
				rvest::html_attr("href")
			)
		talks = talks |> filter(!is.na(url) & (url |> stringr::str_detect("(html|htm)$")))
		if(nrow(talks)==0)next
		
		talks["title"]=NA_character_
		talks["abst"]=NA_character_
		
		
		for(talk.i in 1:nrow(talks)){
			talk.html = rvest::read_html(htmlat(talks$url[talk.i]))
			
			talks$title[talk.i] = talk.html |>
				rvest::html_elements(xpath = '//div[@class="abst_area"]//h4') |> rvest::html_text() |>
				stringr::str_replace_all("【E】","")|>
				stringr::str_replace_all("\n","。")
			
			abst = talk.html |>
				rvest::html_elements(xpath = '//div[@class="abst_text"]//p') |> rvest::html_text()
			
			if(length(abst)>0){
				talks$abst[talk.i] = paste(abst,collapse="")
			}
		}
		
		talk_master = rbind(talk_master,talks)
	}
	readr::write_rds(talk_master,sprintf("ESJ%d_talks.rds",ESJno))
}
}
if(0){
for(ESJno in 63:72){
	data = readr::read_rds(sprintf("ESJ%d_talks.rds",ESJno))
	data = data|>
		dplyr::mutate(code = sprintf("%s-%s",ESJno,name)) |>
		dplyr::select(code,title,abst)
	
	words = NULL
	for(data.i in 1:nrow(data)){
		cat(sprintf("%5d/%5d\t%s\n",data.i,nrow(data),data$code[data.i]))
		
		if(is.na(data$abst[data.i]))next
		abst.mecab = RMeCab::RMeCabC(data$abst[data.i])
		
		type = unlist (abst.mecab)|>names()
		value = unlist (abst.mecab)|>as.character()
		meishi = (type=="名詞") & !stringr::str_detect(value,"^[0-9]+$")
		pair = meishi & c(meishi[-1],FALSE)
		trio = meishi & c(meishi[-1],FALSE) & meishi & c(meishi[-c(1,2)],FALSE,FALSE)
		
		words.this = c(value[meishi],
							paste0(value[pair],value[c(FALSE,pair[-length(pair)])]),
							paste0(value[trio],value[c(FALSE,trio[-length(trio)])],value[c(FALSE,FALSE,trio[-(length(trio)-(1:0))])])) |>
			unique()
		
		if(length(words.this)==0)next
		words = rbind(
			words,
			data.frame(
				code=data$code[data.i],
				word=words.this
			)
		)
	}
	readr::write_rds(words,sprintf("ESJ%d_words.rds",ESJno))
}
}
if(0){
	for(ESJno in 63:72){
		data = readr::read_rds(sprintf("ESJ%d_talks.rds",ESJno))
		data = data|>
			dplyr::mutate(code = sprintf("%s-%s",ESJno,name)) |>
			dplyr::select(code,title,abst)
		
		words = NULL
		for(data.i in 1:nrow(data)){
			cat(sprintf("%5d/%5d\t%s\n",data.i,nrow(data),data$code[data.i]))
			
			if(is.na(data$title[data.i]))next
			title.mecab = RMeCab::RMeCabC(data$title[data.i])
			
			type = unlist (title.mecab)|>names()
			value = unlist (title.mecab)|>as.character()
			meishi = (type=="名詞") & !stringr::str_detect(value,"^[0-9]+$")
			pair = meishi & c(meishi[-1],FALSE)
			trio = meishi & c(meishi[-1],FALSE) & meishi & c(meishi[-c(1,2)],FALSE,FALSE)
			
			words.this = c(value[meishi],
								paste0(value[pair],value[c(FALSE,pair[-length(pair)])]),
								paste0(value[trio],value[c(FALSE,trio[-length(trio)])],value[c(FALSE,FALSE,trio[-(length(trio)-(1:0))])])) |>
				unique()
			
			if(length(words.this)==0)next
			words = rbind(
				words,
				data.frame(
					code=data$code[data.i],
					word=words.this
				)
			)
		}
		readr::write_rds(words,sprintf("ESJ%d_titlewords.rds",ESJno))
	}
}

words = NULL
for(ESJno in 63:72){
	words = rbind(words,
					  readr::read_rds(sprintf("ESJ%d_words.rds",ESJno))
	)
}
codes = words$code|>unique()

word_freq = words|>
	dplyr::filter(!stringr::str_detect(word,"[\\–～㎝・)、ｍ℃土±＋－‐0１２３４５６７８９％①②③④⑤⑥⑦⑧⑨（），．]"))|>
	dplyr::filter(!stringr::str_detect(word,"^[ -~]+$"))|>
	dplyr::group_by(word)|>
	dplyr::summarise(n=dplyr::n()) |>
	dplyr::mutate(prob=(1.0*n)/length(codes))|>
	dplyr::filter(0.01<=prob & prob<=0.1) |>
	dplyr::arrange(prob)

talklist = words |>
	dplyr::filter(word%in%word_freq$word) |>
	dplyr::mutate(value=1)|>
	tidyr::pivot_wider(id_cols=code,names_from=word,values_from=value)

talklist[is.na(talklist)]=0

require("vegan")
bcdist = vegan::vegdist(talklist[,-1],method="jaccard",binary = TRUE)
hc = hclust(bcdist, "ward.D2")

readr::write_rds(bcdist,"ESJ63_72_bcdist.rds")

bcdist = readr::read_rds("ESJ63_72_bcdist.rds")
plot(hc)
rect.hclust(hc, k=26,border = "red")
ct1 = cutree(hc, k = 26, h = NULL)

talklist = talklist |> dplyr::mutate(group=ct1)
group_feature = talklist |> 
	dplyr::select(-1) |>
	dplyr::group_by(group) |> dplyr::summarize( dplyr::across(everything(), sum) ) |>
	dplyr::left_join(data.frame(group=ct1)|>dplyr::group_by(group)|>dplyr::summarise(n=dplyr::n()))|>
	dplyr::relocate(n,.after=1)
feature= character(nrow(group_feature))
for(i in 1:nrow(group_feature)){
	feature[i] = paste(names(group_feature)[-c(1,2)][as.numeric(group_feature[i,-c(1,2)])>as.numeric(group_feature[i,2]*0.1)],collapse=", ")
}


talks = NULL
for(ESJno in 63:72){
	talks = rbind(talks,
					  readr::read_rds(sprintf("ESJ%d_talks.rds",ESJno))|>
					  	dplyr::mutate(code = sprintf("%s-%s",ESJno,name)) |>
					  	dplyr::select(code,title,abst)
					  )
}

talks = talklist |> 
	select(code,group) |>
	dplyr::left_join(talks) |>
	dplyr::arrange(group) |>
	readr::write_excel_csv("group_talks.csv")

group_feature |> select(group,n) |>
	mutate(keyword=feature) |>
	readr::write_excel_csv("group_talks_keywords.csv")



titlewords = NULL
for(ESJno in 63:72){
	titlewords = rbind(titlewords,
					  readr::read_rds(sprintf("ESJ%d_titlewords.rds",ESJno))
	)
}
codes = titlewords$code|>unique()

titleword_freq = titlewords|>
	dplyr::filter(!stringr::str_detect(word,"[\\–～㎝・)、ｍ℃土±＋－‐0１２３４５６７８９％①②③④⑤⑥⑦⑧⑨（），．]"))|>
	dplyr::filter(!stringr::str_detect(word,"^[ -~]+$"))|>
	dplyr::group_by(word)|>
	dplyr::summarise(n=dplyr::n()) |>
	dplyr::mutate(prob=(1.0*n)/length(codes))|>
	dplyr::filter(0.001<=prob & prob<=0.1) |>
	dplyr::arrange(desc(prob))


titletalklist = titlewords |>
	dplyr::filter(word%in%titleword_freq$word) |>
	dplyr::mutate(value=1)|>
	tidyr::pivot_wider(id_cols=code,names_from=word,values_from=value)

titletalklist[is.na(titletalklist)]=0

require("vegan")
titlebcdist = vegan::vegdist(titletalklist[,-1],method="jaccard",binary = TRUE)
titlehc = hclust(titlebcdist, "ward.D2")

readr::write_rds(titlebcdist,"ESJ63_72_titlebcdist.rds")

plot(titlehc)
rect.hclust(titlehc, h=3,border = "red")
ct1 = cutree(titlehc, h = 3, h = NULL)
