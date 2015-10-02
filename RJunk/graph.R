Data_s = read.csv("data_151002_s.csv")
Data_t = read.csv("data_151002_t.csv")
Data=rbind(Data_s,Data_t)


size = 10000


sub = subset(Data, num == size)

names = c("hash","unique","shuffle","select","select with hash")
cols = c("black","red","blue",rgb(0,0.75,0),rgb(0.6,0.6,0))


x.seq = 10^((0:30)/10)
plot(1,0,type="n",xlab="乱数列長に対する乱数範囲の倍率", ylab="time (ms)",xlim=c(1,1000),ylim=c(0,10000),log="x")
for(no in 1:5){
	ssub = subset(sub, type==names[no])
	lines(x.seq[1:length(unique(ssub$max))],ssub$time,col=cols[no],lwd=2)
	points(x.seq[1:length(unique(ssub$max))],ssub$time,col=cols[no],lwd=2)
}

legend("topright", legend = c("hash","sort & unique","shuffle","select","select with hash"), col = c("black","red","blue",rgb(0,0.75,0),rgb(0.6,0.6,0)), pch = c(1,1,1,1,1), lty = c(1,1,1,1,1), lwd=c(2,2,2,2,2))
title(paste("乱数: ",size,"個 × ",10000000/size,"回",sep=""))

