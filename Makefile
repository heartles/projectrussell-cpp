shaders := $(wildcard content/Shaders/*.vert) $(wildcard content/Shaders/*.frag)
outShaders := $(patsubst content/Shaders/%, $(OUTDIR)/%, $(shaders))
levels := $(patsubst content/Levels/%.tmx, $(OUTDIR)/%.json, $(wildcard content/Levels/*.tmx))
artExport := $(patsubst content/Art/%, $(OUTDIR)/%, $(wildcard content/Art/*))
sounds := $(patsubst content/Sound/%, $(OUTDIR)/%, $(wildcard content/Sound/*))
prereqs = dir

ifndef OUTDIR
$(error OUTDIR not set)
endif

.PHONY: dir all clean
# all: shaders $(artExport) $(levels) $(sounds)

content: $(levels) dir
	cp content/Shaders/* $(OUTDIR)
	cp content/Sound/* $(OUTDIR)
	cp content/Art/* $(OUTDIR)
	cp content/testfont.ttf $(OUTDIR)

# TODO: figure out why incremental builds aren't working
# For now it's okay though, since building content takes ~2 sec

dir: 
	mkdir -p $(OUTDIR)

shaders: $(outShaders) $(prereqs)

$(OUTDIR)/%: content/Shaders/% $(prereqs)
	@cp $< $@ -v

$(OUTDIR)/%: content/Sound/% $(prereqs)
	@cp $< $@ -v

$(OUTDIR)/%: content/Art/% $(prereqs)
	@cp $< $@ -v

$(OUTDIR)/%.json: content/Levels/%.tmx $(prereqs)
	tiled --export-map $< $@

clean: $(prereqs)
	rm -rf $(OUTDIR)