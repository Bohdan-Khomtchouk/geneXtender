utils::globalVariables(c("V1", "V2"))
#' Formats a peaks input file.
#'
#' Takes your tab-delimited 3-column input file (see Arguments section below) consisting of peaks called from a peak caller (e.g., MACS2 or SICER) and sorts the file by chromosome and start position to create a formatted file for downstream geneXtendeR analysis.  In this formatted file, the X chromosome is designated by the integer 100, the Y chromosome by the integer 200, and the mitochondrial chromosome by the integer 300.
#'
#' @param filename Name of file containing peaks.  This tab-delimited input file may only contain 3 columns: chromosome number, peak start, and peak end.  See ?sample_peaks_input for more details.
#'
#' @return Returns a formatted file to be used with barChart(), linePlot(), and other downstream commands.
#'
#' @examples
#' ?sample_peaks_input  #Documentation of some exemplary sample input
#' data(sample_peaks_input)
#' head(sample_peaks_input)
#' tail(sample_peaks_input)
#' \dontrun{peaksInput("some_peaks_file.txt")}  #E.g., "my_filename.txt" provided in the /vignettes directory
#'
#' @import data.table
#' 
#' @export
peaksInput <- function(filename) {
	file.input <- fread(filename)
    file.input[V1 == "X", V1 := 100]
    file.input[V1 == "Y", V1 := 200]
    file.input[V1 == "MT", V1 := 300]
    file.input[V1 == "MtDNA", V1 := 300]
    file.input[V1 == "M", V1 := 300]
    file.input[V1 == "Mito", V1 := 300]
    file.sorted <- dplyr::arrange(file.input, as.numeric(V1), V2)
    write.table(file.sorted, file = "peaks.txt", quote = FALSE, sep = "\t", row.names = FALSE, col.names = FALSE)
    }