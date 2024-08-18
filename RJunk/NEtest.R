Rcpp::sourceCpp("src/NEtest.cpp")

cnt = 0
fail = NULL
for(i in 1:1000){
	ans = NEtest(runif(1,-1,1),runif(1,-1,1),0.2,0.01,1,100)
	
	if(ans$value<1e-8){
		cnt = cnt + 1
	}else{
		fail = rbind(fail, data.frame(x=ans$x,y=ans$y))
	}
}

cnt

plot(fail$x,fail$y)

