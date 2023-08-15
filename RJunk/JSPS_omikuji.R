#' Throw toast as the OS notification
#' @description Output characters as toast. Available only on Windows. or Mac
#' @param from Notifying app name
#' @param message Notifying message
#' @return system call result.
#' @export
os_notification = function(from,message){
	os_name = Sys.info()[["sysname"]]
	if(os_name=="Windows"){
		message = gsub("\n","`n",message)
		system(
			sprintf(
				"powershell &{$m=\\\"%s\\\";$a='%s';$t=[Windows.UI.Notifications.ToastNotificationManager,Windows.UI.Notifications,ContentType=WindowsRuntime]::GetTemplateContent([Windows.UI.Notifications.ToastTemplateType,Windows.UI.Notifications,ContentType=WindowsRuntime]::ToastText01);$t.GetElementsByTagName('text').Item(0).InnerText=$m;[Windows.UI.Notifications.ToastNotificationManager]::CreateToastNotifier($a).Show($t);}",
				message,
				from
			)
		)
	}else if(os_name=="Darwin"){
		message = gsub("\n"," ",message)
		system(
			sprintf(
				'osascript -e \'display notification "%s" with title "%s"\'',
				message,
				from
			)
		)
	}else{
		warning("hmRLib::os_notification can be called only on Windows.")
	}
	return(invisible(0))
}


os_notification(
	"日本学術振興会（JSPS）", 
	sprintf("あなたが申請した科学研究費助成事業の審査結果は「%s」です。審査結果の詳細については、以下のリンクからご覧ください。",
		ifelse(runif(1)<0.35,ifelse(runif(1)>0.8,"全会一致採択 満額支給","採択"),"不採択")))
