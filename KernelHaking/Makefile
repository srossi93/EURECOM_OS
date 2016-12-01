TEX=*.tex
PDF=$(TEX:.tex=.pdf)

all: $(PDF)

$(PDF): $(TEX)
	pdflatex -halt-on-error $^ && open -a Preview $@ & 

