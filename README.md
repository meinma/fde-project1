# FDE Project 1
This is the implementation of the first project of the course Foundations of Data Engineering taking place at TU München. It's about the efficient implementation of joins on the TPC-H dataset.

## Dataset
This works on the TPC-H dataset.

## Goal
Given a marketsegment of a customer compute the average of corresponding lineitem quantities. This was a project to practice the effcient implementation of joins. The files had to be read manually. The solution was only accepted if a time measurement better than 15 seconds was achieved and the result was correct.

## Performance Evaluation
The performance evaluation will run the binary compiled from main.cpp so that the avg query is run for multiple market segments on a TPC-H dataset with scale factor 1. The overall runtime from start to end is measured. Details about the machine this will be executed on: Intel(R) Core(TM) i7-3930K CPU @ 3.20GHz with 6 cores, 12 hyperthreads and 64GB of memory.
