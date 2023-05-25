# 林君微生物相互作用解析の可能性検証
require("tidyverse")

# 組み合わせもありで、targetに最も近づくcandidateの組み合わせを探す
brute_force_combination = function(target,candidate,return_num=10){
	AlwaysNo = candidate>target
	AlwaysYes = target*0.1>candidate
	cand = candidate[!(AlwaysNo|AlwaysYes)]
	
	combination = gtools::permutations(n=2,r=length(cand),v=c(FALSE,TRUE),repeats.allowed = TRUE)
	combval = apply(combination,1,function(x){sum(cand[x])})
	
	topX = head(order((combval - target)^2),return_num)
	combination[topX,]
	combval[topX]
}

# 微生物種の比率を定義
True.Sp = rlnorm(10,sdlog = 1.2) %>% sort(decreasing = TRUE)
True.Sp = True.Sp / sum(True.Sp)
barplot(True.Sp)

# 微生物 相互作用を定義
True.Rule = list(
	c(3,2,4),
	c(3,2,-4),
	c(3,-2,1,5),
	c(3,-2,1,-5),
	c(3,-2,-1),
	c(-3,2,7),
	c(-3,2,-7),
	c(-3,-2,8),
	c(-3,-2,-8)
)

# 実験におけるサンプル当たりセル数
True.CellNum = 4.25

#	行方向が各パターン、列方向が各微生物種Spの在不在条件（1 or -1)　0はいてもいなくても影響しない
True.RuleMx = matrix(0,nrow = length(True.Rule), ncol=length(True.Sp))
for(i in 1:length(True.Rule)){
	Rule = True.Rule[[i]]
	Include =Rule[Rule>0]
	Exclude = -Rule[Rule<0]
	True.RuleMx[i,Include] = 1
	True.RuleMx[i,Exclude] = -1
}
	
# ここから真の状態（True）に基づいて、サンプリングを行う

# 実際に100繰り返しで実験生成
ContainProb = 1 - (1 - True.Sp)^True.CellNum
Smpl.Pattern = integer(100)
Smpl.Include = matrix(FALSE,nrow=length(Smpl.Pattern),ncol=ncol(True.RuleMx))
for(i in 1:length(Obsb.Pattern)){
	IsInclude = runif(length(ContainProb),0,1)>ContainProb
	Inc = IsInclude*1 - (!IsInclude)*1
	
	Cand = apply(True.RuleMx * matrix(Inc,nrow=nrow(True.RuleMx),ncol=ncol(True.RuleMx),byrow=TRUE) >= 0, 1, all) 
	Smpl.Include[i,] = Cand
	Smpl.Pattern[i] = (1:length(Cand))[Cand]
}

#全サンプルの10％以上程度を占めないと、パターンとして認識できないと仮定
Smpl.Table = data.frame(no = Smpl.Pattern) %>%
	group_by(no) %>% summarise(num=n()) %>%
	filter(num>length(Smpl.Pattern)*0.1) %>%
	arrange(desc(num))

True.Answer = True.Rule[Smpl.Table$no]
Obsb.Sp = Obsb.Sp/sum(Obsb.Sp)
Obsb.Prob = Smpl.Table$num/length(Smpl.Pattern)
Obsb.Include = (True.RuleMx>0)[Smpl.Table$no,]*1
#種の比率にはノイズが乗るとする
Obsb.Sp = True.Sp*rnorm(length(True.Sp),mean = 1,sd = 0.1)



# 使っていいのは以下の情報

# これが観察された群集の比率（除外された10％はこの中にないため、足してもゼロにならない）
Obsb.Prob
# これが行番目の群集に列番目が含まれていたかどうか（0 or 1）
Obsb.Include
# これが各種の比率
Obsb.Sp

# ここから、True.Answerの再現を目指す

Anl.CellNum = True.CellNum #とりあえず適当なセル数
Anl.IncludeProb = 1 - (1-Obsb.Sp)^Anl.CellNum

# 観察された群集パターンを一つずつ順に調べていく
for(i in 1:nrow(Obsb.Include)){
	# target値 簡単のため、ログ値に変換して処理
	Anl.TargetLogProb = log(Obsb.Prob[i])

	# 現在のAnl.CellNumの仮定の下で、確実に含まれる種が出現する確率
	Anl.BaseLogProb = sum(log(Anl.IncludeProb[Obsb.Include[i,]]))
	
	# exclusion条件を加えた場合に加算される確率
	Anl.ExcludeCand = log(1-Anl.IncludeProb)
	Anl.ExcludeCand[Obsb.Include[i,]] = -Inf
	
	# 総当たりでもっとも足りない確率に近づく組み合わせを探る
	brute_force_combination(-Anl.TargetLogProb + Anl.BaseLogProb, -Anl.ExcludeCand)
}