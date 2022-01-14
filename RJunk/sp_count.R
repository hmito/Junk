num = as.integer(rlnorm(50,sd=1,meanlog = 3))
name = sprintf("sp%d",1:length(num))
#write.csv(data.frame(name=name,num=num),"data.csv",row.names = FALSE)
data = data.frame(name=name,num=num)

start = Sys.time()
for(try in 1:100){
	
	data = read.csv("data.csv")
	
	x = data$num
	y = x
	SpNums = integer(sum(y))
	SpNum = 0;
	for (i in 1:length(SpNums)) {
		#ランダムに未発見の個体をサンプル
		spno = sample(1:length(y),1,prob = y/sum(y))
		if (x[spno] == y[spno]) {
			SpNum = SpNum + 1
			#全種を見つけたら、残りも総種数で埋めて終了
			if(SpNum==length(x) && i!=length(SpNums)){
				SpNums[i:length(SpNums)] = SpNum
				break
			}
		}
		#発見された個体を取り除く
		y[spno] = y[spno] - 1
		#i個体捕まえたときの種数を記録
		SpNums[i] = SpNum
	}
	
	plot(SpNums,type="l",xlab = "sample num",ylab= "sp num")
	
}

end = Sys.time()	
print(paste0("Rの場合:",difftime(end, start, units = "secs"),"sec"))

	require("Rcpp")

	sourceCpp("sp_count.cpp")
	
	data = read.csv("data.csv")

	SpNums = countSp(data$num)
	
	plot(SpNums,type="l",xlab = "sample num",ylab= "sp num")
	


start = Sys.time()	#時間計測スタート
end = Sys.time()		#時間計測ストップ
print(paste0("Rcppの場合:",difftime(end, start, units = "secs"),"sec"))
