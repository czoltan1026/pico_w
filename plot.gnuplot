# Set the terminal to PNG and specify the output file
set terminal pngcairo enhanced size 800,900
set output 'plot2.png'
set datafile separator ","

# Set up the layout for multiple plots (3 rows, 1 column)
set multiplot layout 3,1 title "Measured Data Over Time" font ",14"

# Plot Temperature
set xlabel "Time"
set ylabel "Temperature"
set ytics nomirror
set xrange [*:*]
set yrange [20:30]
plot "server3.txt" using 3:4 with linespoints title "Temperature"

# Plot Pressure
set xlabel "Time"
set ylabel "Pressure"
set ytics nomirror
set xrange [*:*]
set yrange [99000:11000]
plot "server3.txt" using 3:5 with linespoints title "Pressure"

# Plot Humidity
set xlabel "Time"
set ylabel "Humidity"
set ytics nomirror
set xrange [*:*]
set yrange [30:90]
plot "server3.txt" using 3:6 with linespoints title "Humidity"

# End multiplot
unset multiplot

# Close the output
set output
