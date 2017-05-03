#draw triangle vector map
triangle_vectormap = function(x, y, vx, vy, xlab="", ylab="", zlab="", vlim = NA, arrow.len=0.05, arrow.headlen=0.10, col = rev(gray.colors(20))){
	ex = c(1,0)
	ey = c(cos(acos(-1)*120/180),sin(acos(-1)*120/180))
	ez = c(cos(acos(-1)*240/180),sin(acos(-1)*240/180))
	
	par(mai=c(0,0,0,0))
	plot(0,0,xlim=c(-0.1,1.1),ylim=c(-0.2,1.0),type = "n", axes=FALSE, xlab="", ylab="")
	lines(c(0,1,0.5,0),c(0,0,sqrt(3)/2,0))
	
	pos1 = x*ex[1] + y*(ex[1]+ey[1])
	pos2 = x*ex[2] + y*(ex[2]+ey[2])
	vec1 = vx*ex[1]+vy*(ex[1]+ey[1])
	vec2 = vx*ex[2]+vy*(ex[2]+ey[2])
	vlen = sqrt(vec1**2+vec2**2)
	if(is.na(vlim)){
		vlim = c(0,max(vlen))
	}
	alen =arrow.len
	
	vcolno = trunc((vlen-vlim[1])/vlim[2]*length(col))+1
	vcolno[vcolno<1]=1
	vcolno[vcolno>length(col)]=length(col)
	arrows(pos1-vec1/vlen*alen/2,pos2-vec2/vlen*alen/2,pos1+vec1/vlen*alen/2,pos2+vec2/vlen*alen/2,length=arrow.headlen,col=col[vcolno])
	
	pos = ex/20+(-ey+ez)/30
	text(pos[1],pos[2],"0")
	pos = 10*ex/20+(-ey+ez)/13
	text(pos[1],pos[2],xlab)
	pos = 19*ex/20+(-ey+ez)/30
	text(pos[1],pos[2],"1")
	pos = ex + ey/20+(-ez+ex)/30
	text(pos[1],pos[2],"0")
	pos = ex + 10*ey/20+(-ez+ex)/13
	text(pos[1],pos[2],ylab)
	pos = ex + 19*ey/20+(-ez+ex)/30
	text(pos[1],pos[2],"1")
	pos = ex + ey + ez/20+(-ex+ey)/30
	text(pos[1],pos[2],"0")
	pos = ex + ey + 10*ez/20+(-ex+ey)/13
	text(pos[1],pos[2],zlab)
	pos = ex + ey + 19*ez/20+(-ex+ey)/30
	text(pos[1],pos[2],"1")
}
to_triangle=function(x,y){
	ex = c(1,0)
	ey = c(cos(acos(-1)*120/180),sin(acos(-1)*120/180))
	ez = c(cos(acos(-1)*240/180),sin(acos(-1)*240/180))
	pos1 = x*ex[1] + y*(ex[1]+ey[1])
	pos2 = x*ex[2] + y*(ex[2]+ey[2])
	
	return(list(tx=pos1,ty=pos2))
}
triangle_points = function(x,y,...){
	t = to_triangle(x,y)
	points(t$tx,t$ty,...)
}
triangle_lines = function(x,y,...){
	t = to_triangle(x,y)
	lines(t$tx,t$ty,...)
}
