source("triangle_vectormap.R")

#payoff functions
#x:rock
#y:scissors
#z:paper
#x > y > z > x
wx_fn = function(x,y){
	G = N*x
	F = N*y
	S = N*(1-x-y)
	return(
		(1-alpha/N)^(G-1) / ((1-alpha/N)^(G-1) * G + (1-alpha/N)^G*(1-beta/N)^(F-1)*F + phi * (1-alpha/N)^G*(1-beta/N)^F * S) * N + b - c_L*(1-(1-alpha/N)^(G-1))
	)
}
wy_fn = function(x,y){
	G = N*x
	F = N*y
	S = N*(1-x-y)
	return(
		(1-alpha/N)^G*(1-beta/N)^(F-1) / ((1-alpha/N)^(G-1) * G + (1-alpha/N)^G*(1-beta/N)^(F-1)*F + phi * (1-alpha/N)^G*(1-beta/N)^F * S) * N + b - c_F*(1-(1-alpha/N)^G*(1-beta/N)^(F-1))
	)
}
wz_fn = function(x,y){
	G = N*x
	F = N*y
	S = N*(1-x-y)
	return(
		phi*(1-alpha/N)^G*(1-beta/N)^F / ((1-alpha/N)^(G-1) * G + (1-alpha/N)^G*(1-beta/N)^(F-1)*F + phi * (1-alpha/N)^G*(1-beta/N)^F * S) * N
	)
}

#choose sampling points
x = rep(seq(0,1,length=24),times=24)
y = rep(seq(0,1,length=24),each=24)

#ignore the case x + y >1 because x+y+z=1 and z>0
s=x+y
x = x[s<=1]
y = y[s<=1]

#average payoff
b = 0.5
c_L = 1.0
c_F = 0.8
alpha = 0.8
beta = 0.5
phi = 1.2
N = 12
wx = wx_fn(x,y)
wy = wy_fn(x,y)
wz = wz_fn(x,y)

wa = wx*x + wy*y + wz*(1-x-y)

#relative payoff
vx=(wx-wa)
vy=(wy-wa)

triangle_vectormap(x,y,vx,vy,	xlab = "group", ylab = "float", zlab = "solitary", col = rev(gray.colors(20,0.0,0.8)))


x = 0.2
y = 0.2
dt= 0.01
x.seq = numeric(0)
y.seq = numeric(0)

for(i in 1:50000){
	x.seq = c(x.seq,x)
	y.seq = c(y.seq,y)
	
	wx = wx_fn(x,y)
	wy = wy_fn(x,y)
	wz = wz_fn(x,y)
	
	wa = wx*x + wy*y + wz*(1-x-y)
	vx=(wx-wa)
	vy=(wy-wa)
	
	x = x + vx*dt
	y = y + vy*dt
	
}

triangle_lines(x.seq,y.seq,col = "red")
triangle_points(x.seq[50000],y.seq[50000],col="red", pch=20)

x = 0.05
y = 0.90
dt= 0.01
x.seq = numeric(0)
y.seq = numeric(0)

for(i in 1:50000){
	x.seq = c(x.seq,x)
	y.seq = c(y.seq,y)
	
	wx = wx_fn(x,y)
	wy = wy_fn(x,y)
	wz = wz_fn(x,y)
	
	wa = wx*x + wy*y + wz*(1-x-y)
	vx=(wx-wa)
	vy=(wy-wa)
	
	x = x + vx*dt
	y = y + vy*dt
	
	if(x+y>1){
		a = x+y
		x = x/a
		y = y/a
	}
}

triangle_lines(x.seq,y.seq,col = "red")
triangle_points(x.seq[50000],y.seq[50000],col="red", pch=20)



x = 0.8
y = 0.1
dt= 0.01
x.seq = numeric(0)
y.seq = numeric(0)

for(i in 1:50000){
	x.seq = c(x.seq,x)
	y.seq = c(y.seq,y)
	
	wx = wx_fn(x,y)
	wy = wy_fn(x,y)
	wz = wz_fn(x,y)
	
	wa = wx*x + wy*y + wz*(1-x-y)
	vx=(wx-wa)
	vy=(wy-wa)
	
	x = x + vx*dt
	y = y + vy*dt
	
	if(x+y>1){
		a = x+y
		x = x/a
		y = y/a
	}
}

triangle_lines(x.seq,y.seq,col = "red")
triangle_points(x.seq[50000],y.seq[50000],col="red", pch=20)

x = 0.9
y = 0.05
dt= 0.01
x.seq = numeric(0)
y.seq = numeric(0)

for(i in 1:50000){
	x.seq = c(x.seq,x)
	y.seq = c(y.seq,y)
	
	wx = wx_fn(x,y)
	wy = wy_fn(x,y)
	wz = wz_fn(x,y)
	
	wa = wx*x + wy*y + wz*(1-x-y)
	vx=(wx-wa)
	vy=(wy-wa)
	
	x = x + vx*dt
	y = y + vy*dt
	
	if(x+y>1){
		a = x+y
		x = x/a
		y = y/a
	}
}

triangle_lines(x.seq,y.seq,col = "red")
triangle_points(x.seq[50000],y.seq[50000],col="red", pch=20)


sx = numeric(0)
sy = numeric(0)
for(N in 1:16){
	x = 0.2
	y = 0.2
	dt= 0.01
	x.seq = numeric(0)
	y.seq = numeric(0)
	
	for(i in 1:50000){
		x.seq = c(x.seq,x)
		y.seq = c(y.seq,y)
		
		wx = wx_fn(x,y)
		wy = wy_fn(x,y)
		wz = wz_fn(x,y)
		
		wa = wx*x + wy*y + wz*(1-x-y)
		vx=(wx-wa)
		vy=(wy-wa)
		
		x = x + vx*dt
		y = y + vy*dt
	}
	sx = c(sx,x)
	sy = c(sy,y)
}
dev.off()

sx[sx>1] = 1
sy[sy<0] = 0
plot(1:16,sx,ylim = c(0,1),type="l")
lines(1:16,sy,col="blue")
lines(1:16,1-sx-sy,col="red")
