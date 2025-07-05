file=tangram_testbed

all:
	latex ${file}.tex 
	latex ${file}.tex 
	dvips -o ${file}.ps ${file}.dvi
	\rm -f *.toc *.aux *.dvi *.lof *.log *.lot *.bbl *.blg 

clean:
	\rm -f *.toc *.aux *.dvi *.ps *.pdf *.lof *.log *.lot *.bbl *.blg 
