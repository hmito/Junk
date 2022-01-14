#please put the directory path of csv files!
load_butokama_flow=function(path){
	numberOfDays <- function(year,month) {
		date = as.Date(sprintf("%04d-%02d-01",year,month))
		m <- format(date, format="%m")
		
		while (format(date, format="%m") == m) {
			date <- date + 1
		}
		
		return(as.integer(format(date - 1, format="%d")))
	}
	
	table = NULL
	files= dir(path)
	for(filename in files){
		print(sprintf("loading %s...",filename))
		if(length(grep("\\d\\d\\d\\dbuto.csv",filename))){
			start_year = as.integer(substr(filename,1,4))
			start_month = 1
			month_num = 12
		}else if(length(grep("\\d\\d\\d\\dbuto_\\d.csv",filename))){
			start_year = as.integer(substr(filename,1,4))
			start_month = 3*as.integer(substr(filename,10,10))-2
			month_num = 3
		}else if(length(grep("\\d\\d\\d\\dbuto_\\d\\d\\d\\d.csv",filename))){
			start_year = as.integer(substr(filename,1,4))
			start_month = 4
			month_num = 12
		}else if(length(grep("\\d\\d\\d\\dbuto_\\d\\d\\d\\dEnd.csv",filename))){
			start_year = as.integer(substr(filename,1,4))
			start_month = 4
			month_num = NA
		}else{
			print(sprintf("==ERROR==: unknown file name format: %s.",filename))
			next		
		}
		
		dat = read.csv(paste0(path,"/",filename),fileEncoding = "SJIS",header=FALSE)
		
		#remove empty column
		while(all(dat[,1]==""| is.na(dat[,1]))){
			dat= dat[,-1]
		}
		
		
		head = integer(0)
		for(i in 1:nrow(dat)){
			if(length(grep("水位",dat[i,]))>27){
				head = c(head,i)
			}
		}
		
		if(is.na(month_num) || length(head)!=month_num){
			print(sprintf("==ERROR==: head line num is %d instead of %d in %s.",length(head),month_num,filename))
			print(paste0("   head line: ",paste(head,collapse = ", ")))
		}
		
		for(month.i in 1:length(head)){
			success=TRUE
	
			year = start_year + as.integer((start_month+month.i-2)/12)
			month = (start_month+month.i-2)%%12+1
			days = 1:numberOfDays(year,month)
			
			#month check
			adjust = 0
			for(adjust in 0:3){
				adjust.ok = TRUE
				for(hour in 1:24){
					if(length(grep(sprintf("^%d",hour),dat[head[month.i]+hour+adjust,1]))!=1){
						adjust.ok = FALSE
						break
					}
				}
				if(adjust.ok)break
			}
			if(!adjust.ok){
				print(sprintf("==ERROR==: fail to find correct time column at %d.%d in %s.",year,month,filename))
				print(sprintf("   [^%d*]!= [%s]",hour,dat[head[month.i]+hour,1]))
				break 
			}
	
			#data check
			field = dat[head[month.i]+1:24+adjust,2:(2*max(days)+1)]
			numeric_check = (is.na(apply(field,c(1,2),as.numeric)) & field!="")
			if(any(numeric_check)){
				print(sprintf("==ERROR==: numeric check find incorrect field at %d.%d in %s.",year,month,filename))
				break
			}
		
			for(day in days){
				jday = as.integer(as.Date(sprintf("%04d-%02d-%02d",year,month,day))-as.Date(sprintf("%04d-01-01",year)))+1
				table = rbind(table,data.frame(year=rep(year,24),month=rep(month,24),day=rep(day,24),jday=rep(jday,24),hour=1:24,
														 level = field[1:24,day*2-1],
														 flow = field[1:24,day*2]))
			}
		}
	}
	write.csv(table,sprintf("%s.csv",path))
}

#data load
table = load_butokama_flow("D:/hmito/Downloads/buto/data")

# plot ---------------------------------------------------------------------
plot.level = function(data, start_date, end_date){
	table = data %>% filter(start.date<=date & date<=end.date)
	plot(table$date,table$flow,type="n",xlim=c(start.date,end.date),xaxt="n")
	if(as.integer(end.date-start.date)>=365*2){
		pos = as.Date(sprintf("%d-01-01",table$year))
		axis(1, pos, format(pos, "%Y"), cex.axis = 1.0)
		for(i in pos){
			lines(rep(i,2),c(-100,1000))
		}
	}else if(as.integer(end.date-start.date)>=120){
		pos = as.Date(sprintf("%d-%d-01",table$year,table$month))
		axis(1, pos, format(pos, "%Y.%b"), cex.axis = 1.0)
		for(i in pos){
			lines(rep(i,2),c(-100,1000))
		}
	}else{
		axis(1, table$date, format(table$date, "%b/%d"), cex.axis = 1.0)
	}
	lines(table$datetime,table$flow,col="blue",lwd=2)
}

require(tidyverse)
master = table %>% mutate(date =  as.Date(sprintf("%04d-%02d-%02d",table$year,table$month,table$day)), datetime=  as.integer(as.Date(sprintf("%04d-%02d-%02d",table$year,table$month,table$day)))+(1:24)/24)

start.date = as.Date("1987-05-15")
end.date = as.Date("1987-05-20")

plot.level(master,start.date,end.date)

