# Setting --------------------
reset
set terminal gif animate delay 6 enhanced size 1280, 720
set output "montecarlo.gif"
set nokey
set size ratio 1
set margins 0, 0, 0, 0
set origin 0., 0.05  #0. = 0.0 (float)
set size 0.9, 0.9
 
# Parameter --------------------
N = 1e6
cnt = 0
png = 0
 
# Function (Label) --------------------
Counter(i) = sprintf("%-8d", i)
Approx(cnt, i) = sprintf("%-1.6f", 4.*cnt/i) #4. = 4.0 (float)
 
# Plot --------------------
# Draw gray circle
set object 1 circle at 0, 0 size 1 fc rgb 'gray80' fs transparent solid 0.15 noborder
do for [j=1:820]{
 if(j<=100){
  i = j
 } else {
  k = j-100  
  i = 10**((k-1)/180+2) + 5*((k-1)%180+1)*10**((k-1)/180)
 }
 
 # Set range for using stats command
 set xrange [0:i]
 set yrange [0:i]
 
 stats 'data.dat' using 4 every ::(i-1)::(i-1) nooutput
 cnt = STATS_max
 # Displey labels
 set label 1 "N = ".Counter(i) left at screen 0.75, 0.56 font 'Times New Roman:Normal, 28'
 set label 2 "X = ".Counter(cnt) left at screen 0.75, 0.50 font 'Times New Roman:Normal, 28'
 set label 3 "{/symbol:Italic p}  = ".Approx(cnt, i) left at screen 0.75, 0.44 font 'Times New Roman:Normal, 28'
 
 # Set range for plotting
 set xrange [0:1]
 set yrange [0:1]
 plot 'data.dat' using 1:($3==1 ? $2 : 1/0) every ::0::(i-1) with points pt 7 ps 0.4 lc rgb 'royalblue',\
      'data.dat' using 1:($3==0 ? $2 : 1/0) every ::0::(i-1) with points pt 7 ps 0.4 lc rgb 'orange'
 
}
 
set out