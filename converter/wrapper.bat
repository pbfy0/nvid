cd %~dp0
.\ffmpeg.exe -i "%1" -r 24 -t 60 -vcodec libvpx -vf "scale='if(gt(a,4/3),320,-1)':'if(gt(a,4/3),-1,240)',pad=320:240:(320-in_w)/2:(240-in_h)/2" output.ivf
pause