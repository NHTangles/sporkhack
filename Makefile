#	NetHack Makefile.
#	SCCS Id: @(#)Makefile.top	3.4	1995/01/05

# newer makes predefine $(MAKE) to 'make' and do smarter processing of
# recursive make calls if $(MAKE) is used
# these makes allow $(MAKE) to be overridden by the environment if someone
# wants to (or has to) use something other than the standard make, so we do
# not want to unconditionally set $(MAKE) here
#
# unfortunately, some older makes do not predefine $(MAKE); if you have one of
# these, uncomment the following line
# (you will know that you have one if you get complaints about unable to
# execute things like 'data' and 'rumors')
# MAKE = make

# make NetHack
PREFIX	 = /opt/nethack/hardfought.org
GAME     = sporkhack
# GAME     = nethack.prg
GAMEUID  = games
GAMEGRP  = games

# Permissions - some places use setgid instead of setuid, for instance
# See also the option "SECURE" in include/config.h
GAMEPERM = 0755
FILEPERM = 0644
EXEPERM  = 0755
DIRPERM  = 0755

# GAMEDIR also appears in config.h as "HACKDIR".
# VARDIR may also appear in unixconf.h as "VAR_PLAYGROUND" else GAMEDIR
#
# note that 'make install' believes in creating a nice tidy GAMEDIR for
# installation, free of debris from previous NetHack versions --
# therefore there should not be anything in GAMEDIR that you want to keep
# (if there is, you'll have to do the installation by hand or modify the
# instructions)
GAMEDIR  = $(PREFIX)/sporkhack
VARDIR  = $(GAMEDIR)/var
SHELLDIR = /tmp

# per discussion in Install.X11 and Install.Qt
VARDATND = 
# VARDATND = x11tiles NetHack.ad pet_mark.xbm
# VARDATND = x11tiles NetHack.ad pet_mark.xbm rip.xpm
# for Atari/Gem
# VARDATND = nh16.img title.img GEM_RSC.RSC rip.img
# for BeOS
# VARDATND = beostiles
# for Gnome
# VARDATND = x11tiles pet_mark.xbm rip.xpm mapbg.xpm

VARDATD = data oracles options quest.dat rumors
VARDAT = $(VARDATD) $(VARDATND)

# Some versions of make use the SHELL environment variable as the shell
# for running commands.  We need this to be a Bourne shell.
# SHELL = /bin/sh
# for Atari
# SHELL=E:/GEMINI2/MUPFEL.TTP

# Commands for setting the owner and group on files during installation.
# Some systems fail with one or the other when installing over NFS or for
# other permission-related reasons.  If that happens, you may want to set the
# command to "true", which is a no-op. Note that disabling chown or chgrp
# will only work if setuid (or setgid) behavior is not desired or required.
CHOWN = chown
CHGRP = chgrp

#
# end of configuration
#

DATHELP = help hh cmdhelp history opthelp wizhelp

SPEC_LEVS = asmodeus.lev baalz.lev bigrm-*.lev castle.lev fakewiz?.lev hellfill.lev \
	vlt-*.lev \
	juiblex.lev knox.lev medusa-?.lev minend-?.lev minefill.lev \
	minetn-?.lev oracle.lev orcus.lev sanctum.lev soko?-?.lev \
	tower?.lev valley.lev wizard?.lev \
	astral.lev air.lev earth.lev fire.lev water.lev
QUEST_LEVS = ???-goal.lev ???-fil?.lev ???-loca.lev ???-strt.lev

DATNODLB = $(VARDATND) license
DATDLB = $(DATHELP) dungeon $(SPEC_LEVS) $(QUEST_LEVS) $(VARDATD) vaults.dat
DAT = $(DATNODLB) $(DATDLB)

$(GAME):
	( cd src ; $(MAKE) )

all:	$(GAME) recover Guidebook $(VARDAT) dungeon spec_levs check-dlb
	@echo "Done."

# Note: many of the dependencies below are here to allow parallel make
# to generate valid output

Guidebook:
	( cd doc ; $(MAKE) Guidebook )

manpages:
	( cd doc ; $(MAKE) manpages )

data: $(GAME)
	( cd dat ; $(MAKE) data )

rumors: $(GAME)
	( cd dat ; $(MAKE) rumors )

oracles: $(GAME)
	( cd dat ; $(MAKE) oracles )

#	Note: options should have already been made with make, but...
options: $(GAME)
	( cd dat ; $(MAKE) options )

quest.dat: $(GAME)
	( cd dat ; $(MAKE) quest.dat )

spec_levs: dungeon
	( cd util ; $(MAKE) lev_comp )
	( cd dat ; $(MAKE) spec_levs )
	( cd dat ; $(MAKE) quest_levs )

dungeon: $(GAME)
	( cd util ; $(MAKE) dgn_comp )
	( cd dat ; $(MAKE) dungeon )

nhtiles.bmp: $(GAME)
	( cd dat ; $(MAKE) nhtiles.bmp )

x11tiles: $(GAME)
	( cd util ; $(MAKE) tile2x11 )
	( cd dat ; $(MAKE) x11tiles )

beostiles: $(GAME)
	( cd util ; $(MAKE) tile2beos )
	( cd dat ; $(MAKE) beostiles )

NetHack.ad: $(GAME)
	( cd dat ; $(MAKE) NetHack.ad )

pet_mark.xbm:
	( cd dat ; $(MAKE) pet_mark.xbm )

rip.xpm:
	( cd dat ; $(MAKE) rip.xpm )

mapbg.xpm:
	(cd dat ; $(MAKE) mapbg.xpm )

nhsplash.xpm:
	( cd dat ; $(MAKE) nhsplash.xpm )

nh16.img: $(GAME)
	( cd util ; $(MAKE) tile2img.ttp )
	( cd dat ; $(MAKE) nh16.img )

rip.img:
	( cd util ; $(MAKE) xpm2img.ttp )
	( cd dat ; $(MAKE) rip.img )
GEM_RSC.RSC:
	( cd dat ; $(MAKE) GEM_RSC.RSC )

title.img:
	( cd dat ; $(MAKE) title.img )

check-dlb: options
	@if egrep -s librarian dat/options ; then $(MAKE) dlb ; else true ; fi

dlb: spec_levs
	( cd util ; $(MAKE) dlb )
	( cd dat ; ../util/dlb cf nhdat $(DATDLB) )

# recover can be used when INSURANCE is defined in include/config.h
# and the checkpoint option is true
recover: $(GAME)
	( cd util ; $(MAKE) recover )

dofiles:
	target=`sed -n					\
		-e '/librarian/{' 			\
		-e	's/.*/dlb/p' 			\
		-e	'q' 				\
		-e '}' 					\
	  	-e '$$s/.*/nodlb/p' < dat/options` ;	\
	$(MAKE) dofiles-$${target-nodlb}
	cp src/$(GAME) $(GAMEDIR)
	cp util/recover $(GAMEDIR)
	-rm -f $(SHELLDIR)/$(GAME)
	sed -e 's;/usr/games/lib/nethackdir;$(GAMEDIR);' \
		-e 's;HACKDIR/nethack;HACKDIR/$(GAME);' \
		< sys/unix/nethack.sh \
		> $(SHELLDIR)/$(GAME)
# set up their permissions
	-( cd $(GAMEDIR) ; $(CHOWN) $(GAMEUID) $(GAME) recover ; \
			$(CHGRP) $(GAMEGRP) $(GAME) recover )
	chmod $(GAMEPERM) $(GAMEDIR)/$(GAME)
	chmod $(EXEPERM) $(GAMEDIR)/recover
	-$(CHOWN) $(GAMEUID) $(SHELLDIR)/$(GAME)
	$(CHGRP) $(GAMEGRP) $(SHELLDIR)/$(GAME)
	chmod $(EXEPERM) $(SHELLDIR)/$(GAME)

dofiles-dlb: check-dlb
	( cd dat ; cp nhdat $(DATNODLB) $(GAMEDIR) )
# set up their permissions
	-( cd $(GAMEDIR) ; $(CHOWN) $(GAMEUID) nhdat $(DATNODLB) ; \
			$(CHGRP) $(GAMEGRP) nhdat $(DATNODLB) ; \
			chmod $(FILEPERM) nhdat $(DATNODLB) )

dofiles-nodlb:
# copy over the game files
	( cd dat ; cp $(DAT) $(GAMEDIR) )
# set up their permissions
	-( cd $(GAMEDIR) ; $(CHOWN) $(GAMEUID) $(DAT) ; \
			$(CHGRP) $(GAMEGRP) $(DAT) ; \
			chmod $(FILEPERM) $(DAT) )

update: $(GAME) recover $(VARDAT) dungeon spec_levs
#	(don't yank the old version out from under people who're playing it)
	-mv $(GAMEDIR)/$(GAME) $(GAMEDIR)/$(GAME).old
#	quest.dat is also kept open and has the same problems over NFS
#	(quest.dat may be inside nhdat if dlb is in use)
	-mv $(GAMEDIR)/quest.dat $(GAMEDIR)/quest.dat.old
	-mv $(GAMEDIR)/nhdat $(GAMEDIR)/nhdat.old
# set up new versions of the game files
	( $(MAKE) dofiles )
# touch time-sensitive files
	-touch -c $(VARDIR)/bones* $(VARDIR)/?lock* $(VARDIR)/wizard*
	-touch -c $(VARDIR)/save/*
	touch $(VARDIR)/perm $(VARDIR)/record
# and a reminder
	@echo You may also want to install the man pages via the doc Makefile.

update: $(GAME) recover $(VARDAT) dungeon spec_levs


install: $(GAME) recover $(VARDAT) dungeon spec_levs
# set up the directories
# not all mkdirs have -p; those that don't will create a -p directory
	-mkdir -p $(SHELLDIR)
	-rm -rf $(GAMEDIR) $(VARDIR)
	-mkdir -p $(GAMEDIR) $(VARDIR) $(VARDIR)/save
	-rmdir ./-p
	-$(CHOWN) $(GAMEUID) $(GAMEDIR) $(VARDIR) $(VARDIR)/save
	$(CHGRP) $(GAMEGRP) $(GAMEDIR) $(VARDIR) $(VARDIR)/save
	chmod $(DIRPERM) $(GAMEDIR) $(VARDIR) $(VARDIR)/save
# set up the game files
	( $(MAKE) dofiles )
# set up some additional files
	touch $(VARDIR)/perm $(VARDIR)/record $(VARDIR)/logfile $(VARDIR)/xlogfile
	-( cd $(VARDIR) ; $(CHOWN) $(GAMEUID) perm record logfile xlogfile ; \
			$(CHGRP) $(GAMEGRP) perm record logfile xlogfile ; \
			chmod $(FILEPERM) perm record logfile xlogfile )
# and a reminder
	@echo You may also want to reinstall the man pages via the doc Makefile.


# 'make clean' removes all the .o files, but leaves around all the executables
# and compiled data files
clean:
	( cd src ; $(MAKE) clean )
	( cd util ; $(MAKE) clean )

# 'make spotless' returns the source tree to near-distribution condition.
# it removes .o files, executables, and compiled data files
spotless:
	( cd src ; $(MAKE) spotless )
	( cd util ; $(MAKE) spotless )
	( cd dat ; $(MAKE) spotless )
	( cd doc ; $(MAKE) spotless )
