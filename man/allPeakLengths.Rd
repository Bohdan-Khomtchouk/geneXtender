% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/allPeakLengths.r
\name{allPeakLengths}
\alias{allPeakLengths}
\title{Produces box-and-whisker plot showing distribution of peak lengths across a peaks input file.}
\usage{
allPeakLengths(filename)
}
\arguments{
\item{filename}{Name of peaks input file.}
}
\value{
Returns a box-and-whisker plot of peak length distribution across a peaks file.
}
\description{
Makes boxplots of all peak lengths (within a peaks input file) to show how lengths of individual peaks are distributed across the entire peak set.
}
\examples{
myfile <- system.file("extdata", "somepeaksfile.txt", package="geneXtendeR")
allPeakLengths(myfile)

}
