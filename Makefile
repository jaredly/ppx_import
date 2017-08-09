build:
	jbuilder build @install

test:
	jbuilder runtest

pin:
	opam pin add .

repin: build
	opam upgrade ppx_import

build-all:
	jbuilder build --workspace jbuild-workspace.dev @install

test-all:
	jbuilder build --workspace jbuild-workspace.dev @runtest

.PHONY: build test pin repin build-all test-all
