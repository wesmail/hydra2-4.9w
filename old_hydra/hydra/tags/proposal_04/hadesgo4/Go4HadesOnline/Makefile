include $(GO4SYS)/Makefile.config

MYINCLUDE     = -I $(HADDIR_HADESGO4)/Go4HadesClient -I $(HADDIR_HADESGO4) -I $(HADDIR_RICH)

CXXFLAGS     += $(ROOTCFLAGS) \
		-I$(GO4SYS) \
		-I$(QTDIR)/include \
		-I$(GO4SYS)/qtroot/interface \
		-I$(GO4SYS)/Go4GUIBase \
		$(MYINCLUDE) 

THISLIB      = $(HADDIR_HADESGO4)/Go4Library/libGo4HadesOnline.so


HYDRALIBS    =  $(HADDIR_BASE_LIB)/libHydra.so \
		$(HADDIR_ORA_LIB)/libOra.so \
		$(HADDIR_RICH_LIB)/libRich.so \
		$(HADDIR_MDC_LIB)/libMdc.so \
		$(HADDIR_MDCUTIL_LIB)/libMdcUtil.so \
		$(HADDIR_PID_LIB)/libPid.so \
		$(HADDIR_HADESGO4_LIB)/libHadesGo4.so \
		$(HADDIR_MDCGARFIELD_LIB)/libMdcGarfield.so \
		$(HADDIR_TOF_LIB)/libTof.so \
		$(HADDIR_MDCTRACKG_LIB)/libMdcTrackG.so \
		$(HADDIR_MDCTRACKD_LIB)/libMdcTrackD.so \
		$(HADDIR_MDCTRACKS_LIB)/libMdcTrackS.so \
		$(HADDIR_SHOWER_LIB)/libShower.so \
		$(HADDIR_SHOWERTOFINO_LIB)/libShowerTofino.so \
		$(HADDIR_TOFINO_LIB)/libTofino.so \
		$(HADDIR_TOF_LIB)/libTof.so \
		$(HADDIR_START_LIB)/libStart.so \
		$(HADDIR_TRIGGER_LIB)/libTrigger.so \
		$(HADDIR_EVTSERVER_LIB)/libRevt.so \
		$(GSIDIR)/libhrevcli.so \
		$(HADDIR_KICKPLANE_LIB)/libKick.so\
		$(HADDIR_BASE_LIB)/libHydra.so


GO4LIBS     += $(HYDRALIBS)  

##########################################
#Configuration area
#########################################
# Hydra include files
HYDRAINC     = 	-I$(HADDIR_BASE)/datasource \
		-I$(HADDIR_BASE)/datastruct \
		-I$(HADDIR_BASE)/eventhandling \
		-I$(HADDIR_BASE)/geometry  \
		-I$(HADDIR_BASE)/runtimedb \
		-I$(HADDIR_BASE)/util \
                -I$(HADDIR_BASE)/correlation \
                -I$(HADDIR_REVT) \
		-I$(HADDIR_EVTSERVER)\
                -I$(HADDIR_TOF) \
                -I$(HADDIR_KICKPLANE)\
                -I$(HADDIR_MDC) \
                -I$(HADDIR_HADESGO4) \
                -I$(HADDIR_MDCGARFIELD) \
                -I$(HADDIR_RICH) \
                -I$(HADDIR_SHOWER) \
                -I$(HADDIR_START) \
                -I$(HADDIR_TRIGGER) \
                -I$(HADDIR_TOFINO) \
                -I$(HADDIR_ORA) \
                -I$(HADDIR_SHOWERTOFINO)

CXXFLAGS      += $(HYDRAINC)

LIBNAME =       Go4HadesOnline
LIBH    = 	TGo4HadesOnlineStatus.$(HedSuf) \
		TGo4HadesOnlineSlots.$(HedSuf) \
		TGo4HadesOnlinePanelSlots.$(HedSuf) \
		TGo4HadesOnlinePanelStatus.$(HedSuf)\
		TGo4HadesConfigSlots.$(HedSuf) \
		TGo4HadesConfigStatus.$(HedSuf) \
		TGo4StartOnlineSlots.$(HedSuf) \
		TGo4StartOnlineStatus.$(HedSuf)\
		TGo4TriggerOnlineSlots.$(HedSuf) \
		TGo4TriggerOnlineStatus.$(HedSuf)\
		TGo4RichOnlineSlots.$(HedSuf) \
		TGo4RichOnlineStatus.$(HedSuf)\
		TGo4MdcOnlineSlots.$(HedSuf) \
		TGo4MdcOnlineStatus.$(HedSuf)\
		TGo4TofOnlineSlots.$(HedSuf) \
		TGo4TofOnlineStatus.$(HedSuf)\
		TGo4TofinoOnlineSlots.$(HedSuf) \
		TGo4TofinoOnlineStatus.$(HedSuf)\
		TGo4ShowerOnlineSlots.$(HedSuf) \
		TGo4ShowerOnlineStatus.$(HedSuf) \
		TGo4PhysicsOnlineSlots.$(HedSuf) \
		TGo4PhysicsOnlineStatus.$(HedSuf)
         
LIBL     = T$(LIBNAME)LinkDef.$(HedSuf)
LIBS     = $(LIBH:.$(HedSuf)=.$(SrcSuf))
LIBO     = $(LIBS:.$(SrcSuf)=.$(ObjSuf))
LIBDS    = $(LIBNAME)Dict.$(SrcSuf)
LIBDH    = $(LIBDS:.$(SrcSuf)=.$(HedSuf))
LIBDO    = $(LIBDS:.$(SrcSuf)=.$(ObjSuf))
LIBLIB   = lib$(LIBNAME).$(DllSuf)
LIBLIBM  = lib$(LIBNAME).$(DllSuf).$(MAJOR)
LIBLIBMM = lib$(LIBNAME).$(DllSuf).$(MAJOR).$(MINOR)
LIBDEP   = $(LIBS:.$(SrcSuf)=.$(DepSuf))
##########################################

PROGRAMS = $(LIBLIB)

lib: libroot install 

libroot:	$(PROGRAMS) 

qtgui:        
	@echo "Generating Qt part of the GUI..."
	qmake Go4QtHadesOnline.pro -o Makefile.qt ;
	make -f Makefile.qt ;
	@echo "QT finished"

install:
#		@cp -fd *.so* $(GO4SYS)/Go4Library/.

example: qtgui

all:  qtgui lib install qtgui

depend:
	touch Make-depend
	$(MAKEDEP) -fMake-depend -Y -- $(CXXFLAGS) -- $(GO4FITGUIS) $(TESTS) > /dev/null 2>&1

###--------------------------------------------------------------------------

include Make-depend

test:	
	@echo LIBLIB=\"$(LIBLIB)\"
	
clean:
	@rm -f $(HADDIR_HADESGO4)/Go4Library/*libGo4HadesOnline.so* 
	@rm -f $(HADDIR_HADESGO4)/Go4Library/*libGo4QtHadesOnline.so
	@rm -f $(LIBO) $(LIBDS) $(LIBDH) $(LIBDO) $(LIBDEP) core *~ $(LIBLIBM) $(LIBLIBMM)
	@make -f Makefile.qt clean;


$(LIBLIB): $(LIBO) $(LIBDO)
	@echo "Generating ONLINELIB part of the GUI..."
	$(LD) $(SOFLAGS)$(LIBLIBM) $(LDFLAGS) $(LIBO) $(LIBDO) $(OutPutOpt) $(LIBLIBMM) $(HYDRALIBS) 
	$(LN) -sf $(LIBLIBMM) $(LIBLIBM)
	$(LN) -sf $(LIBLIBM) $(LIBLIB)
	@mv *libGo4HadesOnline.so*  $(HADDIR_HADESGO4)/Go4Library 
	@mv *libGo4QtHadesOnline.so* $(HADDIR_HADESGO4)/Go4Library
	@echo Library $(LIBLIB) done.

$(LIBDS): $(LIBH) $(LIBL)
	@echo Generating dictionary $(LIBDS)
	@$(ROOTSYS)/bin/rootcint -f $(LIBDS) -c -p -I$(GO4SYS) $(HYDRAINC) $(LIBH) $(LIBL)

#########################################
%.$(ObjSuf):	%.$(SrcSuf)
	$(CXX) $(CXXFLAGS) $(HYDRAINC) -c $<

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(HedSuf) 
