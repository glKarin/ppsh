#!/bin/sh

mplayer -nofs -slave -identify -vo xv:ck-method=bg:ck=set -colorkey 2114 -ao pulse -http-header-fields 'Referer: https://www.bilibili.com,User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.84 Safari/537.36' -wid 65011746  'http://upos-hz-mirrorcosu.acgvideo.com/upgcxcode/11/46/85114611/85114611-1-30015.m4s?e=ig8euxZM2rNcNbdlhoNvNC8BqJIzNbfqXBvEqxTEto8BTrNvN0GvT90W5JZMkX_YN0MvXg8gNEVEuxTEto8i8o859r1qXg8xNEVE5XREto8GuFGv2U7SuxI72X6fTr859r1qXg8gNEVE5XREto8z5JZC2X2gkX5L5F1eTX1jkXlsTXHeux_f2o859IB_&deadline=1556236929&gen=playurl&nbs=1&oi=1904324726&os=cosu&platform=pc&trid=76269b180de64df69a3f4a6dfdee42b7&uipk=5&upsig=42afdae100ff758e51f622f556425887&uparams=e,deadline,gen,nbs,oi,os,platform,trid,uipk'
