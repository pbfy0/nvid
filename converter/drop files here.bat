cd %~dp0
.\ffmpeg.exe -i "%~f1" -r 24 -vcodec libvpx -vf "scale='if(gt(a,4/3),320,-1)':'if(gt(a,4/3),-1,240)',pad=320:240:(320-in_w)/2:(240-in_h)/2" -f ivf %~n1.ivf.tns
