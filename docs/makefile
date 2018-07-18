all:
	mkdir -p build
	pdflatex -synctex=1 -interaction=nonstopmode --shell-escape --output-directory=./build main.tex
	pdflatex -synctex=1 -interaction=nonstopmode --shell-escape --output-directory=./build main.tex
	pdflatex -synctex=1 -interaction=nonstopmode --shell-escape --output-directory=./build main.tex

clean:
	rm -rf build

run:
	make
	evince build/main.pdf
bib:
	bibtex build/%.aux
html:
	mkdir -p html
	pandoc -s  --listings --latexmathml main.tex -o html/docs.html
