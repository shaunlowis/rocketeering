# like sub_dyn_plot but extandable.
# Will use a class subDynPlot that has a list of the lines, their names, x/y axis names. 
# you then have an array of subDynPlots that gets looped through to init and when the animation function is called.
# one ani function updates all plots
# Will need a dict that has the most recent sensor data that eaach subDynPlot accesses by name to get new data.