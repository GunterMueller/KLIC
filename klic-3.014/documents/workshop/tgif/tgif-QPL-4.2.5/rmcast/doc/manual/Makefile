
all:
	latex tgwb.tex && bibtex tgwb && latex tgwb.tex && latex tgwb.tex && dvips -o tgwb.ps -t a4 tgwb.dvi && gv tgwb.ps

pdf:
	latex tgwb.tex && bibtex tgwb && latex tgwb.tex && latex tgwb.tex && dvipdfm -o tgwb.pdf -p a4 tgwb.dvi && acroread tgwb.pdf

clean:
	\rm -f *.toc *.aux *.dvi *.ps *.pdf *.lof *.log *.lot *.bbl *.blg 
