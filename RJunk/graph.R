Data_s = read.csv("data_151002_s.csv")
Data_t = read.csv("data_151002_t.csv")
Data=rbind(Data_s,Data_t)

sub = subset(Data, num == 1000000)

#names=c("unique1","unique2","unique3")
names = c("hash","shuffle","select","unique","select with hash")
cols = c("black","blue","green","red", "gray")

plot(0,0,type="n",xlab="range", ylab="time (ms)",xlim=c(0,length(unique(sub$max))),ylim=c(0,10000))
for(no in 1:5){
	ssub = subset(sub, type==names[no])
	lines(1:length(unique(ssub$max)),ssub$time,col=cols[no],lwd=2)
	points(1:length(unique(ssub$max)),ssub$time,col=cols[no],lwd=2)
}

10^1.4

