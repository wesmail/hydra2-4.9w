//*-- AUTHOR : J. Markert

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
// HTool
// Tool Class
////////////////////////////////////////////////////////////////////////////
using namespace std;
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <wordexp.h>
#include <execinfo.h> // for backtrace
#include <fstream>
#include "htool.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TString.h"
#include "TObjString.h"
#include "TList.h"
#include "TString.h"
#include "TROOT.h"
#include "TObject.h"
#include "TKey.h"
#include "TDataMember.h"
#include "TAxis.h"
#include "TBaseClass.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TTreeFormula.h"
#include "TLeaf.h"
#include "TGraphErrors.h"
#include "TProfile.h"
#include "TSystem.h"
ClassImp(HTool)

HTool::HTool(const char* name,const char* title)
    : TNamed(name,title)
{
    // constructor for HTool
    initVariables();
}
HTool::~HTool()
{
  // destructor of HTool
}
void HTool::initVariables()
{
    // inits all variables
}

//--------------------------Files---------------------------------------

Bool_t HTool::open(TFile** file,TString fName,TString option)
{
    // Opens root file "fName" to the pointer "file" with option
    // New,Read,Recreate and Update. Checks if file exists or is
    // already opened. If everything works fine kTRUE is returned.
    if(*file)
    {   // checking if file is opened
        if((*file)->IsOpen()){
	    cout<<"Error(HTool::open() , SPECIFIED INPUT FILE IS ALREADY OPEN!"<<endl;
            return kFALSE;
	}else{
	    *file=0;
	}
    }
    option.ToLower();
    if(option.CompareTo("read")==0){   // read from file
        *file = (TFile*)gROOT->GetListOfFiles()->FindObject(fName.Data());
	if (!*file){
	    *file= new TFile(fName.Data(),"READ");
	    cout<<"HTool::open()     : Reading from "<<fName.Data()<<endl;
            return kTRUE;
	}else{
	    cout<<"Error(HTool::open() , SPECIFIED INPUT FILE DOES NOT EXIST!"<<endl;;
	    return kFALSE;
	}
    }
    else if(option.CompareTo("update")==0){   // read from file
	*file = (TFile*)gROOT->GetListOfFiles()->FindObject(fName.Data());
	if (!*file){
	    *file= new TFile(fName.Data(),"UPDATE");
	    cout<<"HTool::open()     : Updating "<<fName.Data()<<endl;
            return kTRUE;
	}else{
	    cout<<"Error(HTool::open() , SPECIFIED INPUT FILE DOES NOT EXIST!"<<endl;
	    return kFALSE;
	}
    }
    else if(option.CompareTo("recreate")==0){
	*file= new TFile(fName.Data(),"RECREATE");
        return kTRUE;
    }
    else if(option.CompareTo("create")==0){
	*file= new TFile(fName.Data(),"CREATE");
        return kTRUE;
    }
    else if(option.CompareTo("new")==0){
	*file= new TFile(fName.Data(),"NEW");
        return kTRUE;
    }
    else {
	cout<<"Error(HTool::open() , UNKOWN OPTION!"<<endl;
	return kFALSE;
    }
}
Bool_t HTool::close(TFile** file)
{
    // Close the root file of the pointer file if
    // existing and open and sets the file pointer to 0.
    // Returns kTRUE if OK.
    TString opt;
    if(*file){
	if((*file)->IsOpen()){
            opt=(*file)->GetOption();
	    if(opt.CompareTo("READ")!=0)(*file)->Save();
	    (*file)->Close();
            *file=0;
	    return kTRUE;
	}else{
	    cout<<"Warning(HTool::close() , SPECIFIED FILE IS NOT OPEN!"<<endl;
	    return kTRUE;
	}
    }else{
	cout<<"Error(HTool::close() , SPECIFIED FILE POINTER IS ZERO!"<<endl;
	return kFALSE;
    }
}
Bool_t HTool::openAscii(FILE** file,TString fName,TString option)
{
    // Opens ascii file "fName" to the pointer file with the option
    // "r" and "w". Returns kTRUE if OK.
    if(*file)
    {   // checking if file is opened
	cout<<"Error(HTool::openAscii() , SPECIFIED INPUT FILE IS MAYBE ALREADY OPEN!"<<endl;
	return kFALSE;
    }

    option.ToLower();
    if(option.CompareTo("r")==0){   // read from file
	*file = (FILE*)gROOT->GetListOfFiles()->FindObject(fName.Data());
	if (!*file){
	    *file= fopen(fName.Data(),"r");
	    cout<<"HTool::openAscii()     : Reading from "<<fName.Data()<<endl;
	    return kTRUE;
	}else{
	    cout<<"Error(HTool::openAscii() , SPECIFIED INPUT FILE DOES NOT EXIST!"<<endl;;
	    return kFALSE;
	}
    }
    else if(option.CompareTo("w")==0){
	*file=fopen(fName.Data(),"w");
	cout<<"HTool::openAscii()     : Writing to "<<fName.Data()<<endl;
	return kTRUE;
    }
    else {
	cout<<"Error(HTool::openAscii() , UNKOWN OPTION!"<<endl;
	return kFALSE;
    }
}
Bool_t HTool::closeAscii(FILE** file)
{
    // Close ascii file of the pointer file if file!=0 and
    // sets the file pointr to 0 afterwards. Returns kTRUE
    // if OK.
    if(*file){
	fclose(*file);
        *file=0;
	return kTRUE;
    }else{
	cout<<"Error(HTool::closeAsii() , SPECIFIED FILE POINTER IS ZERO!"<<endl;
	return kFALSE;
    }
}
TObjArray* HTool::glob(TString pattern)
{
   // Expand 'pattern' as it would be done by the shell (see sh(1)):
   // '*', '?', '~', '$ENVIRONMENT_VARIABLE' and '[', ']' are expanded. In case
   // the expansion is successful, a TObjArray of TObjStrings with the
   // names of all existing files is returned - otherwise NULL.
   //
   // The returned TObjArray must be deleted by the caller of the function!
   //
   // This example returns all existing .root files in /tmp as well as all .root
   // files in your home directory starting with the characters 'a', 'b', 'c'
   // and 'e':
   //
   // TObjArray* files = HTool::glob( "/tmp/*.root $HOME/[a-c,e]*.root" );
   // if (files)
   // {
   //    TObjString* name = NULL;
   //    TIterator*  file = files->MakeIterator();
   //    while ((name = (TObjString*)file->Next()))
   //    {
   //       Char_t* input = name->GetString.Data(); ...
   //    }
   //    delete files;
   // }

   wordexp_t  file_list;
   Char_t**   file;
   TObjArray* filenames = NULL;

   if (pattern.IsNull())
      return NULL;

   if (::wordexp( pattern.Data(), &file_list, 0 ))
   {
      ::wordfree( &file_list );
      return NULL;
   }

   file      = file_list.we_wordv;
   filenames = new TObjArray;

   for (UInt_t i = 0; i < file_list.we_wordc; i++)
   {
      // check if files real exist, since ::wordexp(3) returns the unexpanded
      // pattern, if e.g. the path does not exist
      if (!gSystem->AccessPathName( file[i] ))
	 filenames->Add( new TObjString( file[i] ) );
   }

   ::wordfree( &file_list );
   if (filenames->GetEntries() == 0)
   {
      delete filenames;
      filenames = NULL;
   }

   return filenames;
}


Bool_t HTool::writeNtupleToAscii(TString Input ,
				 TString ntuple,
				 TString Output,
				 TString separator,
				 TString selection,
				 TString condition,
				 Long64_t entries ,
				 Long64_t startEntry,
				 Int_t   ColWidth   )
{
    // Util function for writing ntuple data to ascii file:
    // Input     : Input File (*.root)
    // ntuple    : name of ntuple in Input File
    // Output    : Output ascii File
    // separator : data field separator (" " or "," or "\t" .....)
    // selection : comma separated list of leaf names ( like x,y,z,mom ...)
    //             - if empty all leafs will be written (default)
    //             - if list contains "-" all leafs but the listed ones will be written
    // condition : like Tree->Draw() selection. Only rows which fulfill the condition will be written
    //             - if empty no condition is applied (default)
    // entries   : number of entries which should be read
    //             - if == -1 all entries are analyzied (default)
    // startEntry: first entry to strt in in ntuple
    // ColWidth  : format width for the data field
    //             - if ==0 no format is applied  (default)

    if(gSystem->AccessPathName(Input.Data())){
	cout<<"HTool::writeNtupleToAscii(): Error::InputFile: "<<Input.Data()<<" has not been found!"<<endl;
	return kFALSE;
    }
    TFile*      in=new TFile(Input.Data(),"READ");
    TNtuple* tuple=(TNtuple*)in->Get(ntuple.Data());
    if(tuple==0){
	cout<<"HTool::writeNtupleToAscii(): Error::Ntuple: "<<ntuple.Data()<<" has not been found!"<<endl;
        return kFALSE;
    }
    //------------------------------------------
    // get List of variables in ntuple,
    // number of variables and Rows (Entries)
    TObjArray* arr=tuple->GetListOfLeaves();
    Int_t nvar=tuple->GetNvar();
    if(entries<0) entries =tuple->GetEntries();
    //------------------------------------------



    TArrayI flag(nvar);
    flag.Reset(0);
    cout<<"selection    --------------"<<endl;

    //------------------------------------------
    // positive or negative List ?
    Bool_t positivList=kTRUE;

    if(selection.Contains("-"))  {
        // use all leafs
        positivList=kFALSE;
        flag.Reset(1);
        selection.ReplaceAll("-","");
    }
    //------------------------------------------
    if(positivList){
	cout<<"This Leafs will be selected:"<<endl;
    }
    else{
	cout<<"This Leafs will be skipped:"<<endl;
    }
    cout<<selection.Data()<<endl;
    cout<<"Condition applied : "<<condition.Data()<<endl;

    //------------------------------------------
    // split the lable list into single
    // values. The TObjArray has to be deleted
    // later.
    TObjArray* useList=selection.Tokenize(",");
    Int_t nselect=useList->GetLast()+1;
    //------------------------------------------


    //------------------------------------------
    // create a list of flags for the
    // columns (variables) which should
    // be written out
    // flag=0 will be ignored
    if(nselect!=0)
    {
	// only if a selection was made
        for(Int_t i=0;i<arr->GetLast()+1;i++)
	{
	    TLeaf* l=(TLeaf*)arr->At(i);
	    for(Int_t j=0;j<useList->GetLast()+1;j++)
	    {
		TObjString* obj=(TObjString*)useList->At(j);
		TString lable=obj->GetString();
		lable.ReplaceAll(" ","");

		if(lable.CompareTo(l->GetName())==0)
		{
		    if(positivList) {
			// mark the leafs from the selection
                        // as used
			flag[i]=1;
		    } else {
			// mark the leafs from the selection
			// as not used
			flag[i]=0;
		    }
		    break;
		}
	    }
	}
    }
    //------------------------------------------

    //------------------------------------------
    // open output stream for writing
    // ascii output
    ofstream out;

    out.open(Output.Data());

    if(!out.is_open())
    {
	cout<<"HTool::writeNtupleToAscii(): Error: Could not open Output File!"<<endl;
	return kFALSE;
    }
    //------------------------------------------


    //------------------------------------------
    // write the header line (names of variables)
    // separated by selected separator
    Int_t written=0;
    for(Int_t i=0;i<nvar;i++)
    {
	if(flag[i]==0) continue;
	TLeaf* l=(TLeaf*)arr->At(i);

	if(written==0)
	{
	    if(ColWidth<=0) out<<l->GetName();
	    else            out<<setw(ColWidth)<<l->GetName();
	}
	else
	{
	    if(ColWidth<=0) out<<separator.Data()<<l->GetName();
            else            out<<separator.Data()<<setw(ColWidth)<<l->GetName();
	}
	written++;
    }
    out<<endl;
    //------------------------------------------

    //------------------------------------------
    // compile selection expression if there is one
    TTreeFormula* select = 0;

    if (condition.CompareTo("")!=0) {
	select = new TTreeFormula("Selection",condition,tuple);
	if (!select) {
            cout<<"HTool::writeNtupleToAscii(): TTreeFormular pointer == 0!"<<endl;
	    return kFALSE;
	}
	if (!select->GetNdim()) {
	    cout<<"HTool::writeNtupleToAscii(): TTreeFormular dimension == 0!"<<endl;
	    delete select;
	    return kFALSE;
	}
    }
    //------------------------------------------

    //------------------------------------------
    // no write the data lines
    // separated by selected separator
    for(Long64_t lines=startEntry;lines<entries;lines++)
    {
  	tuple->GetEntry(lines);

	//------------------------------------------
        // aply condition on the line
	if (select) {
	    select->GetNdata();
	    if (select->EvalInstance(0) == 0) continue;
	}
	//------------------------------------------

	Float_t* dat=tuple->GetArgs();
        written=0;

	for(Int_t j=0;j<nvar;j++){
	    if(flag[j]==0) continue;

	    if(written==0){
		if(ColWidth<=0)out<<dat[j];
		else           out<<setw(ColWidth)<<dat[j];
	    }
	    else
	    {   if(ColWidth<=0) out<<separator.Data()<<dat[j];
		else            out<<separator.Data()<<setw(ColWidth)<<dat[j];
	    }
            written++;
	}
	out<<endl;
    }
    //------------------------------------------

    //------------------------------------------
    // close output and delete temoray variables
    out.close();
    useList->Delete();
    delete useList;
    //------------------------------------------

    return kTRUE;
}

void  HTool::backTrace(Int_t level)
{
    // prints out level steps of function calls
    // before the function call (backtrace)
    // Useful for debugging

    void** array =new void* [level];
    size_t size = backtrace (array, level);

    if(size)
    {
	char **strings;
	strings = backtrace_symbols (array, size);

	printf ("Obtained %zd stack frames.\n", size);

	for (size_t i = 0; i < size; i++)
	{
	    if(gSystem->Which("c++filt","/dev/null")==0)
	    {
                // do nice print out using c++filt from gnu binutils
                TString symbol=strings[i];
		TString lib   =symbol;

		lib.Replace   (lib.First('(')   ,lib.Length()-lib.First('('),"");
		symbol.Replace(0                ,symbol.First('(')+1        ,"");
		symbol.Replace(symbol.First('+'),symbol.Length()            ,"");

		cout<<lib.Data()<<": "<<flush;
		gSystem->Exec(Form("c++filt %s",symbol.Data()));
	    }
	    else
	    {
		// do normal print out
		cout<<strings[i]<<endl;

	    }
	}
        free(strings);
    }else{
	cout<<"HTool::backTrack() : Could not retrieve backtrace information"<<endl;
    }
    delete [] array;
}

//----------------------------Dirs-------------------------------------

TDirectory* HTool::Mkdir(TDirectory *dirOld, const Char_t *c, Int_t i, Int_t p)    //! Makes new Dir, changes to Dir, returns pointer to new Dir
{
    // Function to create subdirectories, where dirold is the pointer of the old Directory,
    // "c" the name of the new Directory an "i" the number (c + i) of the directory."p" gives
    // the formating number of digits filled with 0. If "i"==-99 the numbering part is skipped.
    // Checks if the Directrory exists , if so changes into otherwise create it new. The
    // pointer to the new Directory is returned.

    static Char_t sDir[255];// = new Char_t[strlen(c)+3];
    static Char_t sFmt[10];
    if(i!=-99)
    {
	sprintf(sFmt, "%%s %%0%1ii", p);
	sprintf(sDir, sFmt, c, i);
    }
    else
    {
	sprintf(sDir,"%s",c);
    }
    TDirectory *dirNew=0;
    if(!dirOld->FindKey(sDir))
    {
	dirNew = dirOld->mkdir(sDir);
    }
    else
    {
	dirNew=(TDirectory*)dirOld->Get(sDir);
    }
    dirOld->cd(sDir);
    return dirNew;
}

TDirectory* HTool::changeToDir(TString p)
{
    // Changes into the given path. If the Directory is not existing
    // it will be created. The pointer to the new directory will be returned.
    TDirectory *dirNew=0;
    TString s1=p;
    Ssiz_t len=s1.Length();
    if(len!=0)
    {
	char* mystring=(char*)s1.Data();

	Char_t* buffer;
	TList myarguments;
	TObjString *stemp;
	TString argument;
	Int_t count=0;

	while(1) // find all token in option string and put them to a list
	{
	    if(count==0)
	    {
		buffer=strtok(mystring,"/");
		stemp=new TObjString(buffer);
		myarguments.Add(stemp);
	    }

	    if(!(buffer=strtok(NULL,"/")))break;
	    stemp=new TObjString(buffer);
	    myarguments.Add(stemp);

	    count++;
	}
	TIterator* myiter=myarguments.MakeIterator();

	// iterate over the list of arguments
	while ((stemp=(TObjString*)myiter->Next())!= 0)
	{
	    argument=stemp->GetString();

	    dirNew=HTool::Mkdir(gDirectory,argument.Data(),-99);
	}
    }
    return dirNew;
}
Bool_t HTool::checkDir(TString p,TFile* input)
{
    // Checks if a given path "p" in file "input" exists.
    // The actual directory will not be changed. Returns
    // kTRUE if OK.
    TDirectory* dirSave=gDirectory;

    TDirectory *dirNew=input;
    TString s1=p;
    Ssiz_t len=s1.Length();
    if(len!=0)
    {
	char* mystring=(char*)s1.Data();

	Char_t* buffer;
	TList myarguments;
	TObjString *stemp;
	TString argument;
	Int_t count=0;

	while(1) // find all token in option string and put them to a list
	{
	    if(count==0)
	    {
		buffer=strtok(mystring,"/");
		stemp=new TObjString(buffer);
		myarguments.Add(stemp);
	    }

	    if(!(buffer=strtok(NULL,"/")))break;
	    stemp=new TObjString(buffer);
	    myarguments.Add(stemp);

	    count++;
	}
	TIterator* myiter=myarguments.MakeIterator();

	// iterate over the list of arguments
	while ((stemp=(TObjString*)myiter->Next())!= 0)
	{
	    argument=stemp->GetString();
	    if(dirNew->FindKey(argument.Data()))
	    {
		dirNew->cd(argument.Data());
                dirNew=gDirectory;
	    }
	    else
	    {
		dirSave->cd();
		return kFALSE;
	    }
	}
    }
    dirSave->cd();
    return kTRUE;
}

//------------------------misc--------------------------------

TList*  HTool::getListOfAllDataMembers(TClass* cl)
{
    // return a list of all data members of a class.
    // contains all data members of bass classes.

    TList* list=cl->GetListOfDataMembers();

    TIter next_BaseClass(cl->GetListOfBases());
    TBaseClass *pB;
    while ((pB = (TBaseClass*) next_BaseClass())) {
        if (!pB->GetClassPointer()) continue;
        list->AddAll(pB->GetClassPointer()->GetListOfDataMembers() );
    }
    return list;
}

void HTool::scanOracle(TString inputname,TString outputname)
{
    // scans oracle runs table source code to extract
    // file names, run ids and number of events

    FILE* input =0;
    FILE* output=0;

    if(!HTool::openAscii(&input,inputname  ,"r"))
    {
	exit(1);
    }
    if(!HTool::openAscii(&output,outputname,"w"))
    {
	exit(1);
    }

    Char_t line[4000];

    TString buffer="";
    TString filename;
    TString fileRunId;
    TString fileNEvents;
    TString newLine;

    Int_t count=0;
    Int_t sumEvts=0;
    Int_t Evts=0;

    while(1)
    {
	if(feof(input)) break;
	fgets(line, sizeof(line), input);
        buffer=line;
	if (buffer.Contains("<TD align=\"center\">100"))
	{
            buffer.ReplaceAll("<TD align=\"center\">","");
            buffer.ReplaceAll("</TD>","");
            buffer.ReplaceAll("\n","");
	    fileRunId=buffer;

	    fgets(line, sizeof(line), input);
            fgets(line, sizeof(line), input);
            fgets(line, sizeof(line), input);
            fgets(line, sizeof(line), input);
            buffer=line;
            buffer.ReplaceAll("<TD align=\"center\"><A href=\"hades_www.show_log_run_info.show?search_string=","");
            buffer.Remove(buffer.First("&"));
            buffer.ReplaceAll("\n","");
	    filename=buffer;

	    fgets(line, sizeof(line), input);
	    buffer=line;
            buffer.ReplaceAll("<TD align=\"center\">","");
	    buffer.ReplaceAll("</TD>","");
            buffer.ReplaceAll("\n","");

	    fileNEvents=buffer;
	    newLine= filename + " " + fileRunId + " " + fileNEvents;
            sscanf(newLine.Data(),"%*s%*s%i",&Evts);
	    sumEvts=sumEvts+Evts;
            count++;
            fprintf(output,"%s%s",newLine.Data(),"\n");
	}
    }
    cout<<count<<" Files with "<<sumEvts<<" Events"<<endl;
    fprintf(output,"%i Files with %i Events\n",count,sumEvts);

    HTool::closeAscii(&input);
    HTool::closeAscii(&output);

}
Double_t HTool::roundDigits(Double_t d, Int_t ndigit)
{
    // Round d to ndigits
    if(ndigit<0) return -1.;
    d= pow(10.,-ndigit) * floor( pow(10.,ndigit) * d + 0.5 );
    return d;
}
Float_t HTool::roundDigits(Float_t f, Int_t ndigit)
{
    // Round d to ndigits
    if(ndigit<0) return -1.;
    f= pow(10.,-ndigit) * floor( pow(10.,ndigit) * f + 0.5 );
    return f;
}
Int_t HTool::exec(TString command, TString& output)
{
   // Execute 'command' in a shell and return the exit code of 'command'.
   // 'output' is filled with the data written to STDOUT and STDERR by
   // 'command'.
 
   TString line;
   FILE*   pipe;

   if (command.IsNull())
      return 1;

   command.Prepend( "exec 2>&1; " );
   pipe = gSystem->OpenPipe( command.Data(), "r" );

   output = "";
   while (line.Gets( pipe ))
      output += line;

   return gSystem->ClosePipe( pipe );
}

void HTool::sort(Int_t n, Char_t* arrIn,Int_t* index,Bool_t down,Bool_t overwrite)
{
    // sorts array arrIn. If down=kTRUE sort in decreasing order.
    // If overwrite=kTRUE input array is sorted otherwise not and
    // the result can be obtained from the index array (has to be provided
    // by the caller with size >=n ) If index=NULL the index array is not used.
    // if index=NULL and overwrite=kFALSE the function returns
    // without doing anything.

    if(!overwrite&&!index) return;
    if(n<2)                return;

    register Int_t a,b,c;
    Int_t exchange,bin=0;
    Char_t val;

    Char_t* arr=0;
    Char_t* arrNew=0;

    if(!overwrite) {
	arrNew = new Char_t [n];
        memcpy(arrNew,arrIn,n*sizeof(Char_t));
	arr=arrNew;
    } else {
        arr=arrIn;
    }

    if(index) for(Int_t i=0;i<n;i++) index[i]=i;

    for(a=0;a<n-1;++a)
    {
	exchange = 0;
	c        = a;
	val      = arr[a];

	if(index) bin = index[a];

	for(b=a+1;b<n;++b)
	{
	    if( down&&arr[b]>val)   // > decreasing order
	    {
		c        = b;
		val      = arr[b];
		exchange = 1;

                if(index) bin = index[b];
	    }
	    if(!down&&arr[b]<val)   // < increasing order
	    {
		c        = b;
		val      = arr[b];
		exchange = 1;

                if(index) bin = index[b];
	    }
	}
	if(exchange)
	{
	    arr[c]   = arr[a];
	    arr[a]   = val;

	    if(index)
	    {
		index[c] = index[a];
		index[a] = bin;
	    }

	}
    }
    if(!overwrite) delete [] arrNew;
}

void HTool::sort(Int_t n, Short_t* arrIn,Int_t* index,Bool_t down,Bool_t overwrite)
{
    // sorts array arrIn. If down=kTRUE sort in decreasing order.
    // If overwrite=kTRUE input array is sorted otherwise not and
    // the result can be obtained from the index array (has to be provided
    // by the caller with size >=n ) If index=NULL the index array is not used.
    // if index=NULL and overwrite=kFALSE the function returns
    // without doing anything.

    if(!overwrite&&!index) return;
    if(n<2)                return;

    register Int_t a,b,c;
    Int_t exchange,bin=0;
    Short_t val;

    Short_t* arr=0;
    Short_t* arrNew=0;

    if(!overwrite) {
	arrNew = new Short_t [n];
        memcpy(arrNew,arrIn,n*sizeof(Short_t));
	arr=arrNew;
    } else {
        arr=arrIn;
    }

    if(index) for(Int_t i=0;i<n;i++) index[i]=i;

    for(a=0;a<n-1;++a)
    {
	exchange = 0;
	c        = a;
	val      = arr[a];

	if(index) bin = index[a];

	for(b=a+1;b<n;++b)
	{
	    if( down&&arr[b]>val)   // > decreasing order
	    {
		c        = b;
		val      = arr[b];
		exchange = 1;

                if(index) bin = index[b];
	    }
	    if(!down&&arr[b]<val)   // < increasing order
	    {
		c        = b;
		val      = arr[b];
		exchange = 1;

                if(index) bin = index[b];
	    }
	}
	if(exchange)
	{
	    arr[c]   = arr[a];
	    arr[a]   = val;

	    if(index)
	    {
		index[c] = index[a];
		index[a] = bin;
	    }

	}
    }
    if(!overwrite) delete [] arrNew;
}


void HTool::sort(Int_t n, Int_t* arrIn,Int_t* index,Bool_t down,Bool_t overwrite)
{
    // sorts array arrIn. If down=kTRUE sort in decreasing order.
    // If overwrite=kTRUE input array is sorted otherwise not and
    // the result can be obtained from the index array (has to be provided
    // by the caller with size >=n ) If index=NULL the index array is not used.
    // if index=NULL and overwrite=kFALSE the function returns
    // without doing anything.

    if(!overwrite&&!index) return;
    if(n<2)                return;

    register Int_t a,b,c;
    Int_t exchange,bin=0;
    Int_t val;

    Int_t* arr=0;
    Int_t* arrNew=0;

    if(!overwrite) {
	arrNew = new Int_t [n];
        memcpy(arrNew,arrIn,n*sizeof(Int_t));
	arr=arrNew;
    } else {
        arr=arrIn;
    }

    if(index) for(Int_t i=0;i<n;i++) index[i]=i;

    for(a=0;a<n-1;++a)
    {
	exchange = 0;
	c        = a;
	val      = arr[a];

	if(index) bin = index[a];

	for(b=a+1;b<n;++b)
	{
	    if( down&&arr[b]>val)   // > decreasing order
	    {
		c        = b;
		val      = arr[b];
		exchange = 1;

                if(index) bin = index[b];
	    }
	    if(!down&&arr[b]<val)   // < increasing order
	    {
		c        = b;
		val      = arr[b];
		exchange = 1;

                if(index) bin = index[b];
	    }
	}
	if(exchange)
	{
	    arr[c]   = arr[a];
	    arr[a]   = val;

	    if(index)
	    {
		index[c] = index[a];
		index[a] = bin;
	    }

	}
    }
    if(!overwrite) delete [] arrNew;
}

void HTool::sort(Int_t n, Float_t* arrIn,Int_t* index,Bool_t down,Bool_t overwrite)
{
    // sorts array arrIn. If down=kTRUE sort in decreasing order.
    // If overwrite=kTRUE input array is sorted otherwise not and
    // the result can be obtained from the index array (has to be provided
    // by the caller with size >=n ) If index=NULL the index array is not used.
    // if index=NULL and overwrite=kFALSE the function returns
    // without doing anything.

    if(!overwrite&&!index) return;
    if(n<2)                return;

    register Int_t a,b,c;
    Int_t exchange,bin=0;
    Float_t val;

    Float_t* arr=0;
    Float_t* arrNew=0;

    if(!overwrite) {
	arrNew = new Float_t [n];
        memcpy(arrNew,arrIn,n*sizeof(Float_t));
	arr=arrNew;
    } else {
        arr=arrIn;
    }

    if(index) for(Int_t i=0;i<n;i++) index[i]=i;

    for(a=0;a<n-1;++a)
    {
	exchange = 0;
	c        = a;
	val      = arr[a];

	if(index) bin = index[a];

	for(b=a+1;b<n;++b)
	{
	    if( down&&arr[b]>val)   // > decreasing order
	    {
		c        = b;
		val      = arr[b];
		exchange = 1;

                if(index) bin = index[b];
	    }
	    if(!down&&arr[b]<val)   // < increasing order
	    {
		c        = b;
		val      = arr[b];
		exchange = 1;

                if(index) bin = index[b];
	    }
	}
	if(exchange)
	{
	    arr[c]   = arr[a];
	    arr[a]   = val;

	    if(index)
	    {
		index[c] = index[a];
		index[a] = bin;
	    }

	}
    }
    if(!overwrite) delete [] arrNew;
}

void HTool::sort(Int_t n, Double_t* arrIn,Int_t* index,Bool_t down,Bool_t overwrite)
{
    // sorts array arrIn. If down=kTRUE sort in decreasing order.
    // If overwrite=kTRUE input array is sorted otherwise not and
    // the result can be obtained from the index array (has to be provided
    // by the caller with size >=n ) If index=NULL the index array is not used.
    // if index=NULL and overwrite=kFALSE the function returns
    // without doing anything.

    if(!overwrite&&!index) return;
    if(n<2)                return;

    register Int_t a,b,c;
    Int_t exchange,bin=0;
    Double_t val;

    Double_t* arr=0;
    Double_t* arrNew=0;

    if(!overwrite) {
	arrNew = new Double_t [n];
        memcpy(arrNew,arrIn,n*sizeof(Double_t));
	arr=arrNew;
    } else {
        arr=arrIn;
    }

    if(index) for(Int_t i=0;i<n;i++) index[i]=i;

    for(a=0;a<n-1;++a)
    {
	exchange = 0;
	c        = a;
	val      = arr[a];

	if(index) bin = index[a];

	for(b=a+1;b<n;++b)
	{
	    if( down&&arr[b]>val)   // > decreasing order
	    {
		c        = b;
		val      = arr[b];
		exchange = 1;

                if(index) bin = index[b];
	    }
	    if(!down&&arr[b]<val)   // < increasing order
	    {
		c        = b;
		val      = arr[b];
		exchange = 1;

                if(index) bin = index[b];
	    }
	}
	if(exchange)
	{
	    arr[c]   = arr[a];
	    arr[a]   = val;

	    if(index)
	    {
		index[c] = index[a];
		index[a] = bin;
	    }

	}
    }
    if(!overwrite) delete [] arrNew;
}

void HTool::sortParallel(Int_t n,Int_t nArrays,Char_t* arrIn,Int_t leading,Int_t* index,Bool_t down)
{
    // sorts n arrays arrIn paralell (destruktive!). Array pointer arrIn[leading] will be the
    // leading one. The other arrays will be sorted according to the reordering of the leading array.
    // If down=kTRUE sort in decreasing order.
    // The result can be obtained from the index array (has to be provided
    // by the caller with size >=n ) If index=NULL the index array is not used.
    // if index=NULL and overwrite=kFALSE the function returns
    // without doing anything.

    if(n<2)  return;

    register Int_t a,b,c;
    Int_t exchange,bin=0;

    Char_t* arr=0;
    Char_t* tempVar =new Char_t [nArrays];

    arr=arrIn;

    // prepare index array
    if(index) {
	for(Int_t i=0;i<n;i++) index[i]=i;
    }


    for(a=0;a<n-1;++a)
    {
	exchange = 0;
	c        = a;

	for(Int_t i=0;i<nArrays;i++){
	    tempVar[i] = arr[i*n+a];
	}

	if(index) bin  = index[a];

	for(b=a+1;b<n;++b)
	{
	    if( down&&arr[leading*n+b]>tempVar[leading])   // > decreasing order
	    {
		c        = b;

		for(Int_t i=0;i<nArrays;i++){
		    tempVar[i] = arr[i*n+b];
		}

		exchange = 1;

                if(index) bin = index[b];
	    }
	    if(!down&&arr[leading*n+b]<tempVar[leading])   // < increasing order
	    {
		c        = b;

		for(Int_t i=0;i<nArrays;i++){
		    tempVar[i] = arr[i*n+b];
		}

		exchange = 1;

                if(index) bin = index[b];
	    }
	}
	if(exchange)
	{

	    for(Int_t i=0;i<nArrays;i++){
		arr[i*n+c]=arr[i*n+a];
		arr[i*n+a]=tempVar[i];
	    }

	    if(index)
	    {
		index[c] = index[a];
		index[a] = bin;
	    }
	}
    }

    delete [] tempVar;

}
void HTool::sortParallel(Int_t n,Int_t nArrays,Short_t* arrIn,Int_t leading,Int_t* index,Bool_t down)
{
    // sorts n arrays arrIn paralell (destruktive!). Array pointer arrIn[leading] will be the
    // leading one. The other arrays will be sorted according to the reordering of the leading array.
    // If down=kTRUE sort in decreasing order.
    // The result can be obtained from the index array (has to be provided
    // by the caller with size >=n ) If index=NULL the index array is not used.
    // if index=NULL and overwrite=kFALSE the function returns
    // without doing anything.

    if(n<2)  return;

    register Int_t a,b,c;
    Int_t exchange,bin=0;

    Short_t* arr=0;
    Short_t* tempVar =new Short_t [nArrays];

    arr=arrIn;

    // prepare index array
    if(index) {
	for(Int_t i=0;i<n;i++) index[i]=i;
    }


    for(a=0;a<n-1;++a)
    {
	exchange = 0;
	c        = a;

	for(Int_t i=0;i<nArrays;i++){
	    tempVar[i] = arr[i*n+a];
	}

	if(index) bin  = index[a];

	for(b=a+1;b<n;++b)
	{
	    if( down&&arr[leading*n+b]>tempVar[leading])   // > decreasing order
	    {
		c        = b;

		for(Int_t i=0;i<nArrays;i++){
		    tempVar[i] = arr[i*n+b];
		}

		exchange = 1;

                if(index) bin = index[b];
	    }
	    if(!down&&arr[leading*n+b]<tempVar[leading])   // < increasing order
	    {
		c        = b;

		for(Int_t i=0;i<nArrays;i++){
		    tempVar[i] = arr[i*n+b];
		}

		exchange = 1;

                if(index) bin = index[b];
	    }
	}
	if(exchange)
	{

	    for(Int_t i=0;i<nArrays;i++){
		arr[i*n+c]=arr[i*n+a];
		arr[i*n+a]=tempVar[i];
	    }

	    if(index)
	    {
		index[c] = index[a];
		index[a] = bin;
	    }
	}
    }

    delete [] tempVar;

}
void HTool::sortParallel(Int_t n,Int_t nArrays,Int_t* arrIn,Int_t leading,Int_t* index,Bool_t down)
{
    // sorts n arrays arrIn paralell (destruktive!). Array pointer arrIn[leading] will be the
    // leading one. The other arrays will be sorted according to the reordering of the leading array.
    // If down=kTRUE sort in decreasing order.
    // The result can be obtained from the index array (has to be provided
    // by the caller with size >=n ) If index=NULL the index array is not used.
    // if index=NULL and overwrite=kFALSE the function returns
    // without doing anything.

    if(n<2)  return;

    register Int_t a,b,c;
    Int_t exchange,bin=0;

    Int_t* arr=0;
    Int_t* tempVar =new Int_t [nArrays];

    arr=arrIn;

    // prepare index array
    if(index) {
	for(Int_t i=0;i<n;i++) index[i]=i;
    }


    for(a=0;a<n-1;++a)
    {
	exchange = 0;
	c        = a;

	for(Int_t i=0;i<nArrays;i++){
	    tempVar[i] = arr[i*n+a];
	}

	if(index) bin  = index[a];

	for(b=a+1;b<n;++b)
	{
	    if( down&&arr[leading*n+b]>tempVar[leading])   // > decreasing order
	    {
		c        = b;

		for(Int_t i=0;i<nArrays;i++){
		    tempVar[i] = arr[i*n+b];
		}

		exchange = 1;

                if(index) bin = index[b];
	    }
	    if(!down&&arr[leading*n+b]<tempVar[leading])   // < increasing order
	    {
		c        = b;

		for(Int_t i=0;i<nArrays;i++){
		    tempVar[i] = arr[i*n+b];
		}

		exchange = 1;

                if(index) bin = index[b];
	    }
	}
	if(exchange)
	{

	    for(Int_t i=0;i<nArrays;i++){
		arr[i*n+c]=arr[i*n+a];
		arr[i*n+a]=tempVar[i];
	    }

	    if(index)
	    {
		index[c] = index[a];
		index[a] = bin;
	    }
	}
    }

    delete [] tempVar;

}
void HTool::sortParallel(Int_t n,Int_t nArrays,Float_t* arrIn,Int_t leading,Int_t* index,Bool_t down)
{
    // sorts n arrays arrIn paralell (destruktive!). Array pointer arrIn[leading] will be the
    // leading one. The other arrays will be sorted according to the reordering of the leading array.
    // If down=kTRUE sort in decreasing order.
    // The result can be obtained from the index array (has to be provided
    // by the caller with size >=n ) If index=NULL the index array is not used.
    // if index=NULL and overwrite=kFALSE the function returns
    // without doing anything.

    if(n<2)  return;

    register Int_t a,b,c;
    Int_t exchange,bin=0;

    Float_t* arr=0;
    Float_t* tempVar =new Float_t [nArrays];

    arr=arrIn;

    // prepare index array
    if(index) {
	for(Int_t i=0;i<n;i++) index[i]=i;
    }


    for(a=0;a<n-1;++a)
    {
	exchange = 0;
	c        = a;

	for(Int_t i=0;i<nArrays;i++){
	    tempVar[i] = arr[i*n+a];
	}

	if(index) bin  = index[a];

	for(b=a+1;b<n;++b)
	{
	    if( down&&arr[leading*n+b]>tempVar[leading])   // > decreasing order
	    {
		c        = b;

		for(Int_t i=0;i<nArrays;i++){
		    tempVar[i] = arr[i*n+b];
		}

		exchange = 1;

                if(index) bin = index[b];
	    }
	    if(!down&&arr[leading*n+b]<tempVar[leading])   // < increasing order
	    {
		c        = b;

		for(Int_t i=0;i<nArrays;i++){
		    tempVar[i] = arr[i*n+b];
		}

		exchange = 1;

                if(index) bin = index[b];
	    }
	}
	if(exchange)
	{

	    for(Int_t i=0;i<nArrays;i++){
		arr[i*n+c]=arr[i*n+a];
		arr[i*n+a]=tempVar[i];
	    }

	    if(index)
	    {
		index[c] = index[a];
		index[a] = bin;
	    }
	}
    }

    delete [] tempVar;

}
void HTool::sortParallel(Int_t n,Int_t nArrays,Double_t* arrIn,Int_t leading,Int_t* index,Bool_t down)
{
    // sorts n arrays arrIn paralell (destruktive!). Array pointer arrIn[leading] will be the
    // leading one. The other arrays will be sorted according to the reordering of the leading array.
    // If down=kTRUE sort in decreasing order.
    // The result can be obtained from the index array (has to be provided
    // by the caller with size >=n ) If index=NULL the index array is not used.
    // if index=NULL and overwrite=kFALSE the function returns
    // without doing anything.

    if(n<2)  return;

    register Int_t a,b,c;
    Int_t exchange,bin=0;

    Double_t* arr=0;
    Double_t* tempVar =new Double_t [nArrays];

    arr=arrIn;

    // prepare index array
    if(index) {
	for(Int_t i=0;i<n;i++) index[i]=i;
    }


    for(a=0;a<n-1;++a)
    {
	exchange = 0;
	c        = a;

	for(Int_t i=0;i<nArrays;i++){
	    tempVar[i] = arr[i*n+a];
	}

	if(index) bin  = index[a];

	for(b=a+1;b<n;++b)
	{
	    if( down&&arr[leading*n+b]>tempVar[leading])   // > decreasing order
	    {
		c        = b;

		for(Int_t i=0;i<nArrays;i++){
		    tempVar[i] = arr[i*n+b];
		}

		exchange = 1;

                if(index) bin = index[b];
	    }
	    if(!down&&arr[leading*n+b]<tempVar[leading])   // < increasing order
	    {
		c        = b;

		for(Int_t i=0;i<nArrays;i++){
		    tempVar[i] = arr[i*n+b];
		}

		exchange = 1;

                if(index) bin = index[b];
	    }
	}
	if(exchange)
	{

	    for(Int_t i=0;i<nArrays;i++){
		arr[i*n+c]=arr[i*n+a];
		arr[i*n+a]=tempVar[i];
	    }

	    if(index)
	    {
		index[c] = index[a];
		index[a] = bin;
	    }
	}
    }

    delete [] tempVar;

}

Double_t HTool::kurtosis(Int_t n, Short_t* data)
{
    // calculates the kurtosis
    //
    //   ---                     //
    //   \                       //
    //   /   ( x-mean)^4         //
    //   ---                     //
    //  ----------------  - 3    //
    //       n*sigma^4           //
    //
    //
    // The kurtosis will be > 0 for distributions
    // compared wider than a normal distribution and
    // and < 0 for distributions beeing peaking stronger

    if(n<1) return -999;

    Double_t mean = TMath::Mean(n,data);
    Double_t sigma= TMath::RMS (n,data);

    Double_t sum=0;
    for(Int_t i=0;i<n;i++){
	sum+= TMath::Power(data[i]-mean,4);
    }
    Double_t kurtosis=(sum/((n)*TMath::Power(sigma,4)))-3;
    return  kurtosis;
}
Double_t HTool::kurtosis(Int_t n, Int_t* data)
{
    // calculates the kurtosis
    //
    //   ---                     //
    //   \                       //
    //   /   ( x-mean)^4         //
    //   ---                     //
    //  ----------------  - 3    //
    //       n*sigma^4           //
    //
    //
    // The kurtosis will be > 0 for distributions
    // compared wider than a normal distribution and
    // and < 0 for distributions beeing peaking stronger

    if(n<1) return -999;

    Double_t mean = TMath::Mean(n,data);
    Double_t sigma= TMath::RMS (n,data);

    Double_t sum=0;
    for(Int_t i=0;i<n;i++){
	sum+= TMath::Power(data[i]-mean,4);
    }
    Double_t kurtosis=(sum/((n)*TMath::Power(sigma,4)))-3;
    return  kurtosis;
}
Double_t HTool::kurtosis(Int_t n, Float_t* data)
{
    // calculates the kurtosis
    //
    //   ---                     //
    //   \                       //
    //   /   ( x-mean)^4         //
    //   ---                     //
    //  ----------------  - 3    //
    //       n*sigma^4           //
    //
    //
    // The kurtosis will be > 0 for distributions
    // compared wider than a normal distribution and
    // and < 0 for distributions beeing peaking stronger

    if(n<1) return -999;

    Double_t mean = TMath::Mean(n,data);
    Double_t sigma= TMath::RMS (n,data);

    Double_t sum=0;
    for(Int_t i=0;i<n;i++){
	sum+= TMath::Power(data[i]-mean,4);
    }
    Double_t kurtosis=(sum/((n)*TMath::Power(sigma,4)))-3;
    return  kurtosis;
}
Double_t HTool::kurtosis(Int_t n, Double_t* data)
{
    // calculates the kurtosis
    //
    //   ---                     //
    //   \                       //
    //   /   ( x-mean)^4         //
    //   ---                     //
    //  ----------------  - 3    //
    //       n*sigma^4           //
    //
    //
    // The kurtosis will be > 0 for distributions
    // compared wider than a normal distribution and
    // and < 0 for distributions beeing peaking stronger

    if(n<1) return -999;

    Double_t mean = TMath::Mean(n,data);
    Double_t sigma= TMath::RMS (n,data);

    Double_t sum=0;
    for(Int_t i=0;i<n;i++){
	sum+= TMath::Power(data[i]-mean,4);
    }
    Double_t kurtosis=(sum/((n)*TMath::Power(sigma,4)))-3;
    return  kurtosis;
}

Double_t HTool::skewness(Int_t n, Short_t* data)
{
    // calculates the skew
    //
    //   ---                     //
    //   \                       //
    //   /   ( x-mean)^3         //
    //   ---                     //
    //  ----------------         //
    //       n*sigma^3           //
    //
    //
    // The skew will be > 0 for distributions
    // haveing more entries in the tails larger than mean
    // and < 0 for distributions haveing more entries small
    // than mean

    if(n<1) return -999;

    Double_t mean = TMath::Mean(n,data);
    Double_t sigma= TMath::RMS (n,data);

    Double_t sum=0;
    for(Int_t i=0;i<n;i++){
	sum+= TMath::Power(data[i]-mean,3);
    }
    Double_t skew=sum/((n)*TMath::Power(sigma,3));
    return  skew;
}
Double_t HTool::skewness(Int_t n, Int_t* data)
{
    // calculates the skew
    //
    //   ---                     //
    //   \                       //
    //   /   ( x-mean)^3         //
    //   ---                     //
    //  ----------------         //
    //       n*sigma^3           //
    //
    //
    // The skew will be > 0 for distributions
    // haveing more entries in the tails larger than mean
    // and < 0 for distributions haveing more entries small
    // than mean

    if(n<1) return -999;

    Double_t mean = TMath::Mean(n,data);
    Double_t sigma= TMath::RMS (n,data);

    Double_t sum=0;
    for(Int_t i=0;i<n;i++){
	sum+= TMath::Power(data[i]-mean,3);
    }
    Double_t skew=sum/((n)*TMath::Power(sigma,3));
    return  skew;
}
Double_t HTool::skewness(Int_t n, Float_t* data)
{
    // calculates the skew
    //
    //   ---                     //
    //   \                       //
    //   /   ( x-mean)^3         //
    //   ---                     //
    //  ----------------         //
    //       n*sigma^3           //
    //
    //
    // The skew will be > 0 for distributions
    // haveing more entries in the tails larger than mean
    // and < 0 for distributions haveing more entries small
    // than mean

    if(n<1) return -999;

    Double_t mean = TMath::Mean(n,data);
    Double_t sigma= TMath::RMS (n,data);

    Double_t sum=0;
    for(Int_t i=0;i<n;i++){
	sum+= TMath::Power(data[i]-mean,3);
    }
    Double_t skew=sum/((n)*TMath::Power(sigma,3));
    return  skew;
}
Double_t HTool::skewness(Int_t n, Double_t* data)
{
    // calculates the skew
    //
    //   ---                     //
    //   \                       //
    //   /   ( x-mean)^3         //
    //   ---                     //
    //  ----------------         //
    //       n*sigma^3           //
    //
    //
    // The skew will be > 0 for distributions
    // haveing more entries in the tails larger than mean
    // and < 0 for distributions haveing more entries small
    // than mean

    if(n<1) return -999;

    Double_t mean = TMath::Mean(n,data);
    Double_t sigma= TMath::RMS (n,data);

    Double_t sum=0;
    for(Int_t i=0;i<n;i++){
	sum+= TMath::Power(data[i]-mean,3);
    }
    Double_t skew=sum/((n)*TMath::Power(sigma,3));
    return  skew;
}

Double_t HTool::weightedMean(Int_t n, Short_t* data,Short_t* dataerr)
{
    // calculates the weighted mean
    //
    //   ---                     //
    //   \                       //
    //   /   x/sigma^2           //
    //   ---                     //
    //  ----------------         //
    //   ---                     //
    //   \                       //
    //   /   1/sigma^2           //
    //   ---                     //
    //
    // where x are the data points from data[i] and
    // sigma the standard deviations of the points from dataerr[i]

    if(n<1) return -999;


    Double_t sum   =0;
    Double_t sumerr=0;
    Double_t w     =0;

    for(Int_t i=0;i<n;i++){
        w=1./(dataerr[i]*dataerr[i]);
	sum   += data[i]*w;
        sumerr+= w;
    }
    if(sumerr!=0) return sum/sumerr;
    else          return -1;
}
Double_t HTool::weightedMean(Int_t n, Int_t* data,Int_t* dataerr)
{
    // calculates the weighted mean
    //
    //   ---                     //
    //   \                       //
    //   /   x/sigma^2           //
    //   ---                     //
    //  ----------------         //
    //   ---                     //
    //   \                       //
    //   /   1/sigma^2           //
    //   ---                     //
    //
    // where x are the data points from data[i] and
    // sigma the standard deviations of the points from dataerr[i]

    if(n<1) return -999;


    Double_t sum   =0;
    Double_t sumerr=0;
    Double_t w     =0;

    for(Int_t i=0;i<n;i++){
        w=1./(dataerr[i]*dataerr[i]);
	sum   += data[i]*w;
        sumerr+= w;
    }
    if(sumerr!=0) return sum/sumerr;
    else          return -1;
}
Double_t HTool::weightedMean(Int_t n, Float_t* data,Float_t* dataerr)
{
    // calculates the weighted mean
    //
    //   ---                     //
    //   \                       //
    //   /   x/sigma^2           //
    //   ---                     //
    //  ----------------         //
    //   ---                     //
    //   \                       //
    //   /   1/sigma^2           //
    //   ---                     //
    //
    // where x are the data points from data[i] and
    // sigma the standard deviations of the points from dataerr[i]

    if(n<1) return -999;


    Double_t sum   =0;
    Double_t sumerr=0;
    Double_t w     =0;

    for(Int_t i=0;i<n;i++){
        w=1./(dataerr[i]*dataerr[i]);
	sum   += data[i]*w;
        sumerr+= w;
    }
    if(sumerr!=0) return sum/sumerr;
    else          return -1;
}
Double_t HTool::weightedMean(Int_t n, Double_t* data,Double_t* dataerr)
{
    // calculates the weighted mean
    //
    //   ---                     //
    //   \                       //
    //   /   x/sigma^2           //
    //   ---                     //
    //  ----------------         //
    //   ---                     //
    //   \                       //
    //   /   1/sigma^2           //
    //   ---                     //
    //
    // where x are the data points from data[i] and
    // sigma the standard deviations of the points from dataerr[i]

    if(n<1) return -999;


    Double_t sum   =0;
    Double_t sumerr=0;
    Double_t w     =0;

    for(Int_t i=0;i<n;i++){
        w=1./(dataerr[i]*dataerr[i]);
	sum   += data[i]*w;
        sumerr+= w;
    }
    if(sumerr!=0) return sum/sumerr;
    else          return -1;
}
Double_t HTool::weightedSigma(Int_t n,Short_t* dataerr)
{
    // calculates the weighted sigma
    //
    //             1                  //
    //sqrt  ----------------          //
    //        ---                     //
    //        \                       //
    //        /   1/sigma^2           //
    //        ---                     //
    //
    // where sigma is the standard deviations of the
    // points from dataerr[i]

    if(n<1) return -999;


    Double_t sumerr=0;
    Double_t w     =0;

    for(Int_t i=0;i<n;i++){
        w=1./(dataerr[i]*dataerr[i]);
        sumerr+= w;
    }
    if(sumerr!=0) return TMath::Sqrt(1./sumerr);
    else          return -1;
}
Double_t HTool::weightedSigma(Int_t n,Int_t* dataerr)
{
    // calculates the weighted sigma
    //
    //             1                  //
    //sqrt  ----------------          //
    //        ---                     //
    //        \                       //
    //        /   1/sigma^2           //
    //        ---                     //
    //
    // where sigma is the standard deviations of the
    // points from dataerr[i]

    if(n<1) return -999;


    Double_t sumerr=0;
    Double_t w     =0;

    for(Int_t i=0;i<n;i++){
        w=1./(dataerr[i]*dataerr[i]);
        sumerr+= w;
    }
    if(sumerr!=0) return TMath::Sqrt(1./sumerr);
    else          return -1;
}
Double_t HTool::weightedSigma(Int_t n,Float_t* dataerr)
{
    // calculates the weighted sigma
    //
    //             1                  //
    //sqrt  ----------------          //
    //        ---                     //
    //        \                       //
    //        /   1/sigma^2           //
    //        ---                     //
    //
    // where sigma is the standard deviations of the
    // points from dataerr[i]

    if(n<1) return -999;


    Double_t sumerr=0;
    Double_t w     =0;

    for(Int_t i=0;i<n;i++){
        w=1./(dataerr[i]*dataerr[i]);
        sumerr+= w;
    }
    if(sumerr!=0) return TMath::Sqrt(1./sumerr);
    else          return -1;
}
Double_t HTool::weightedSigma(Int_t n,Double_t* dataerr)
{
    // calculates the weighted sigma
    //
    //             1                  //
    //sqrt  ----------------          //
    //        ---                     //
    //        \                       //
    //        /   1/sigma^2           //
    //        ---                     //
    //
    // where sigma is the standard deviations of the
    // points from dataerr[i]

    if(n<1) return -999;


    Double_t sumerr=0;
    Double_t w     =0;

    for(Int_t i=0;i<n;i++){
        w=1./(dataerr[i]*dataerr[i]);
        sumerr+= w;
    }
    if(sumerr!=0) return TMath::Sqrt(1./sumerr);
    else          return -1;
}
Double_t HTool::weightedMeanAndSigma(Int_t n, Short_t* data,Short_t* dataerr, Double_t* mean, Double_t* sigma)
{
    // calculates the weighted mean and sigma
    //
    //         ---                    //
    //         \                      //
    //         /   x/sigma^2          //
    //         ---                    //
    // mean = ----------------        //
    //         ---                    //
    //         \                      //
    //         /   1/sigma^2          //
    //         ---                    //
    //
    // where x are the data points from data[i] and
    // sigma the standard deviations of the points from dataerr[i]
    //
    // and
    //
    //                     1                  //
    //sigma = sqrt  ----------------          //
    //                ---                     //
    //                \                       //
    //                /   1/sigma^2           //
    //                ---                     //
    //
    // where sigma is the standard deviations of the
    // points from dataerr[i]


    if(n<1) return -999;


    Double_t sum   =0;
    Double_t sumerr=0;
    Double_t w     =0;

    for(Int_t i=0;i<n;i++){
        w=1./(dataerr[i]*dataerr[i]);
	sum   += data[i]*w;
        sumerr+= w;
    }
    if(sumerr!=0) {
        *sigma=TMath::Sqrt(1./sumerr);
        *mean =sum/sumerr;
	return  0;
    }
    else {

	*sigma=0;
        *mean =0;
     
	return -1;
    }
}
Double_t HTool::weightedMeanAndSigma(Int_t n, Int_t* data,Int_t* dataerr, Double_t* mean, Double_t* sigma)
{
    // calculates the weighted mean and sigma
    //
    //         ---                    //
    //         \                      //
    //         /   x/sigma^2          //
    //         ---                    //
    // mean = ----------------        //
    //         ---                    //
    //         \                      //
    //         /   1/sigma^2          //
    //         ---                    //
    //
    // where x are the data points from data[i] and
    // sigma the standard deviations of the points from dataerr[i]
    //
    // and
    //
    //                     1                  //
    //sigma = sqrt  ----------------          //
    //                ---                     //
    //                \                       //
    //                /   1/sigma^2           //
    //                ---                     //
    //
    // where sigma is the standard deviations of the
    // points from dataerr[i]


    if(n<1) return -999;


    Double_t sum   =0;
    Double_t sumerr=0;
    Double_t w     =0;

    for(Int_t i=0;i<n;i++){
        w=1./(dataerr[i]*dataerr[i]);
	sum   += data[i]*w;
        sumerr+= w;
    }
    if(sumerr!=0) {
        *sigma=TMath::Sqrt(1./sumerr);
        *mean =sum/sumerr;
	return  0;
    }
    else {

	*sigma=0;
        *mean =0;
     
	return -1;
    }
}
Double_t HTool::weightedMeanAndSigma(Int_t n, Float_t* data,Float_t* dataerr, Double_t* mean, Double_t* sigma)
{
    // calculates the weighted mean and sigma
    //
    //         ---                    //
    //         \                      //
    //         /   x/sigma^2          //
    //         ---                    //
    // mean = ----------------        //
    //         ---                    //
    //         \                      //
    //         /   1/sigma^2          //
    //         ---                    //
    //
    // where x are the data points from data[i] and
    // sigma the standard deviations of the points from dataerr[i]
    //
    // and
    //
    //                     1                  //
    //sigma = sqrt  ----------------          //
    //                ---                     //
    //                \                       //
    //                /   1/sigma^2           //
    //                ---                     //
    //
    // where sigma is the standard deviations of the
    // points from dataerr[i]


    if(n<1) return -999;


    Double_t sum   =0;
    Double_t sumerr=0;
    Double_t w     =0;

    for(Int_t i=0;i<n;i++){
        w=1./(dataerr[i]*dataerr[i]);
	sum   += data[i]*w;
        sumerr+= w;
    }
    if(sumerr!=0) {
        *sigma=TMath::Sqrt(1./sumerr);
        *mean =sum/sumerr;
	return  0;
    }
    else {

	*sigma=0;
        *mean =0;
     
	return -1;
    }
}
Double_t HTool::weightedMeanAndSigma(Int_t n, Double_t* data,Double_t* dataerr, Double_t* mean, Double_t* sigma)
{
    // calculates the weighted mean and sigma
    //
    //         ---                    //
    //         \                      //
    //         /   x/sigma^2          //
    //         ---                    //
    // mean = ----------------        //
    //         ---                    //
    //         \                      //
    //         /   1/sigma^2          //
    //         ---                    //
    //
    // where x are the data points from data[i] and
    // sigma the standard deviations of the points from dataerr[i]
    //
    // and
    //
    //                     1                  //
    //sigma = sqrt  ----------------          //
    //                ---                     //
    //                \                       //
    //                /   1/sigma^2           //
    //                ---                     //
    //
    // where sigma is the standard deviations of the
    // points from dataerr[i]


    if(n<1) return -999;


    Double_t sum   =0;
    Double_t sumerr=0;
    Double_t w     =0;

    for(Int_t i=0;i<n;i++){
        w=1./(dataerr[i]*dataerr[i]);
	sum   += data[i]*w;
        sumerr+= w;
    }
    if(sumerr!=0) {
        *sigma=TMath::Sqrt(1./sumerr);
        *mean =sum/sumerr;
	return  0;
    }
    else {

	*sigma=0;
        *mean =0;
     
	return -1;
    }
}

//--------------------------hists-------------------------
void HTool::roundHist(TH2* h,Int_t ndigit,Int_t ndigiterr)
{
    // Round bin content to ndigit and binerror to ndigterr digits.
    // for ndigit or ndigiterr =-1 the rounding will be skipped for
    // the corresponding value.

    if(!h)
    {
	cout<<"HTool::roundHist(): Zero pointer received!"<<endl;
        exit(1);
    }
    Int_t binx=h->GetNbinsX();
    Int_t biny=h->GetNbinsY();
    Double_t val;
    for(Int_t i=1;i<=binx;i++)
    {
	for(Int_t j=1;j<=biny;j++)
	{
	    if(ndigit!=-1)
	    {
		val=h->GetBinContent(i,j);
		h->SetBinContent(i,j,HTool::roundDigits(val,ndigit));
	    }
	    if(ndigiterr!=-1)
	    {
		val=h->GetBinError(i,j);
		h->SetBinError(i,j,HTool::roundDigits(val,ndigiterr));
	    }
	}
    }
}
TH1* HTool::getHist(TFile* inp,TString name)
{
    // gets histogram with name name from root file
    inp->cd();
    TH1* h=(TH1*)inp->Get(name.Data());
    if(h==0)
    {
        cout<<"getHist(): Hist "<<name.Data()<<" does not exist !"<<endl;
        return h;
    }
    return h;
}

TObjArray* HTool::slices(TH2* h2,TF1* f,TString axis,Int_t firstbin,Int_t lastbin,Int_t cut,TString opt,TString suffix,Int_t markerstyle,Int_t markercolor,Float_t markersize)
{
    // calls internal the fitslice of root. Suffix can be added to name of fitted hists.
    // Returns a TObjArray pointer to an array containing the result hists of the fit.
    // Numbering is the same as the the parameters of the fitfunction + chi2 at last place.

    TObjArray* array=0;
    if(h2==0)
    {
        cout<<"Warning: HTool::slices : ZERO pointer retrieved for histogram!"<<endl;
        return array;
    }
    axis.ToLower();
    if     (axis.CompareTo("x")==0) h2->FitSlicesX(f,firstbin,lastbin,cut,opt);
    else if(axis.CompareTo("y")==0) h2->FitSlicesY(f,firstbin,lastbin,cut,opt);
    else
    {
        cout<<"Warning: HTool::slices : Unknown argument for axis : "<<axis.Data()<<"!"<<endl;
        return array;
    }
    Int_t owner=0;
    if(f==0) {f=new TF1("fit","gaus");owner=1;}

    TH1D* h;
    array=new TObjArray(f->GetNpar()+1);
    TString name="";
    name=h2->GetName();
    Char_t histname[300];

    for(Int_t i=0;i<f->GetNpar();i++)
    {
        sprintf(histname,"%s%s%i",name.Data(),"_",i);
        h= (TH1D*)gDirectory->Get(histname);
        if(suffix.CompareTo("")!=0)
        {
            sprintf(histname,"%s%s%s%s%i",name.Data(),"_",suffix.Data(),"_",i);
            h->SetName(histname);
        }
        array->AddAt(h,i);
    }
    sprintf(histname,"%s%s",name.Data(),"_chi2");
    h=(TH1D*)gDirectory->Get(histname);
    if(suffix.CompareTo("")!=0)
    {
        sprintf(histname,"%s%s%s%s",name.Data(),"_",suffix.Data(),"_chi2");
        h->SetName(histname);
    }
    array->AddAt(h,f->GetNpar());

    for(Int_t i=0;i<array->LastIndex()+1;i++)
    {
        h =(TH1D*)array->At(i);
        h->SetLineColor(markercolor);
        h->SetMarkerColor(markercolor);
        h->SetMarkerStyle(markerstyle);
        h->SetMarkerSize(markersize);
    }
    if(owner==1) delete f;
    return array;
}

TObjArray* HTool::projections(TH2* h2,TString axis,Int_t firstbin,Int_t lastbin,Int_t nsteps,TString opt,TString suffix,Int_t markerstyle,Int_t markercolor,Float_t markersize)
{
    // calls internal the projections function of the hist.
    // Suffix can be added to name of fitted hists.
    // Returns a TObjArray pointer to an array containing
    // the result hists of the loop of projections.

    TObjArray* array=0;
    if(h2==0)
    {
        cout<<"Warning: HTool::projections : ZERO pointer retrieved for histogram!"<<endl;
        return array;
    }
    axis.ToLower();

    Int_t stepsize;
    Int_t bin1=firstbin,bin2=lastbin;


    Char_t name[300];
    //---------------------defining range----------------------
    if(firstbin==0||lastbin==-1)
    {   // full range
        bin1=1;
        if(axis.CompareTo("x")==0)bin2=h2->GetNbinsY();
        if(axis.CompareTo("y")==0)bin2=h2->GetNbinsX();
    }else
    {   // part of range
        bin1 = firstbin;
        bin2 = lastbin;
    }
    array=new TObjArray(0);
    //---------------------defining stepsize--------------------
    if(nsteps==-99)stepsize = 1;
    else           stepsize = nsteps;

    //---------------------projecting---------------------------
    for(Int_t i=bin1;i<bin2+1;i+=stepsize)
    {
        if  (axis.CompareTo("x")==0){
            if(suffix.CompareTo("")==0)sprintf(name,"%s%s%i"    ,h2->GetName(),"_px_",i);
            else                       sprintf(name,"%s%s%s%s%i",h2->GetName(),"_",suffix.Data(),"_px_",i);
            array->AddLast(h2->ProjectionX(name,i,i+stepsize-1,opt)); }
        else if(axis.CompareTo("y")==0){
            if(suffix.CompareTo("")==0)sprintf(name,"%s%s%i"    ,h2->GetName(),"_py_",i);
            else                       sprintf(name,"%s%s%s%s%i",h2->GetName(),"_",suffix.Data(),"_py_",i);
            array->AddLast(h2->ProjectionY(name,i,i+stepsize-1,opt)); }
       else{
            cout<<"Warning: HTool::slices : Unknown argument for axis : "<<axis.Data()<<"!"<<endl;
            return array;
        }
    }

    //---------------------setting attributes-------------------
    array->Expand(array->GetLast()+1);
    cout<<"number of hists "<<array->LastIndex()+1<<endl;
    for(Int_t i=0;i<array->LastIndex()+1;i++)
    {
        ((TH1D*)array->At(i))->SetLineColor(markercolor);
        ((TH1D*)array->At(i))->SetMarkerColor(markercolor);
        ((TH1D*)array->At(i))->SetMarkerStyle(markerstyle);
        ((TH1D*)array->At(i))->SetMarkerSize(markersize);
    }
    return array;
}
TObjArray*  HTool::fitArray(TObjArray* array,TF1* fit,TString name,Float_t min,Float_t max,Int_t opt,Float_t x1,Float_t x2,Int_t markerstyle,Int_t markercolor,Float_t markersize)
{
    // Applies fit fit to all hists in array. The result of the fit
    // will be returned in a TObjArray which contains hists for all parameters
    // from fit (with corresponding errors set). The number of hists is equal npar+1.
    // The ordering inside the array starts from par0 to parN and the last hist
    // will contain the chi2. The number of bins in the hists will be equal to the
    // the number of fitted hists. The range of the result hists can be set via
    // min and max. If these values ar not specified the range will be 0-1.
    // If opt=1 fit will use range x1 x2 arround max of hists
    if(array==0){
	cout<<"HTool::fitArray(): array pointer =0!"<<endl;
	return 0;
    }
    if(fit  ==0){
	cout<<"HTool::fitArray(): fit pointer =0!"<<endl;
	return 0;
    }
    if(name.CompareTo("")==0) {
	cout<<"HTool::fitArray(): no name for hists specified!"<<endl;
	return 0;}

    Int_t size       =array->GetEntries();
    Int_t nPars      =fit->GetNpar();
    TObjArray* result=new TObjArray(nPars+1);
    TH1D* htmp=0;
    TString myname=name;

    Int_t nbinsx=size;
    Float_t xmin,xmax;

    if(min==0&&max==0)
    {
	xmin=0.;
	xmax=1.;
    }
    else
    {
 	xmin=min;
	xmax=max;
    }

    for(Int_t i=0;i<nPars+1;i++)
    {
        myname=name;
	if(i!=nPars){
	    myname+="_par";
            myname+=i;
	}
	else
	{
           myname+="_chi2";
	}
	htmp=new TH1D(myname,myname,nbinsx,xmin,xmax);
	htmp->SetLineColor(1);
	htmp->SetMarkerColor(markercolor);
        htmp->SetMarkerStyle(markerstyle);
        htmp->SetMarkerSize (markersize);
	result->AddAt(htmp,i);
    }
    Double_t val=0,valerr=0;
    for(Int_t j=0;j<size;j++)
    {
       TH1* h=(TH1*)array->At(j);
       if(opt==1)
       {
          Float_t mean=h->GetXaxis()->GetBinCenter(h->GetMaximumBin());
          fit->SetRange(mean-x1,mean+x2);
       }

       h->Fit(fit,"QNR");

       if(h->Integral()>2)
       {
	   for(Int_t i=0;i<nPars;i++)
	   {
               htmp=(TH1D*)result->At(i);
	       val   =fit->GetParameter(i);
               valerr=fit->GetParError (i);

	       if(finite(val)   !=0&&
                  finite(valerr)!=0)
	       {
		   htmp->SetBinContent(j+1,val);
		   htmp->SetBinError  (j+1,valerr);
	       }
	   }
	   val=fit->GetChisquare();
	   if(finite(val)!=0)
	   {
	       ((TH1*)result->At(nPars))->SetBinContent(j+1,val);
	   }
       }
    }

    return result;
}
TH1D*  HTool::projectY(TH1* h,Int_t xbin1,Int_t xbin2,TString suff,
                       Int_t ybin,Float_t ymin,Float_t ymax,
                       Int_t markerstyle,Int_t markercolor,Float_t markersize)
{
    if(h==0)
    {
        cout<<"HTool::projectY(): ZERO pointer for hists received!"<<endl;
        return 0;
    }
    TString name ="";
    name=h->GetName();

    TString title="";

    if(suff.CompareTo("")==0) name+="_projY";
    else                      name+=suff;

    if(ybin==0||(ymin==ymax))
    {
        ymin=h->GetMinimum();
        ymax=h->GetMaximum();
        ybin=100;
    }

    if(xbin1<1 )xbin1=1;
    if(xbin2>h->GetNbinsX())xbin2=h->GetNbinsX();

    if(xbin2==0)xbin2=h->GetNbinsX();

    TH1D* hproj=new TH1D(name,title,ybin,ymin,ymax);
    hproj->SetMarkerStyle(markerstyle);
    hproj->SetMarkerColor(markercolor);
    hproj->SetMarkerSize(markersize);
    hproj->SetLineColor(markercolor);
    hproj->SetXTitle(h->GetXaxis()->GetTitle());

    for(Int_t i=xbin1;i<=xbin2;i++)
    {
        hproj->Fill(h->GetBinContent(i));
    }
    return hproj;
}
TGraphErrors* HTool::fitScatter(TH2* h,TF1* f,
				TString opt,
                                Bool_t silent,
				Float_t xmin,Float_t xmax,
				Float_t ymin,Float_t ymax,
				Float_t window,Bool_t clean)
{
    // Fits h with TF1 f. Convert the hist to TGraphErrors
    // first and than perfom the fit on the graph.
    // With xmin/xmax/ymin,ymax a region can be selected.
    // If window is !=0 the graph is fitted with f and
    // in an second iteration the graph is rebuild to just
    // contain values inside the window arround f. If clean
    // is true all values of the hist arround the window of
    // f are set to 0. String opt are options for the Fit.
    // silent=kTRUE switches the prints off.

    if(h==0) return 0;
    if(f==0) return 0;

    Bool_t quiet=silent;
    if(!quiet)
    {
	cout<<"----------------------------------------------------"<<endl;
	cout<<"HTool:fitScatter(): fitting "<<h->GetName()<<endl;
	cout<<"----------------------------------------------------"<<endl;
    }
    Bool_t usewindow=kFALSE;
    if(window!=0)usewindow=kTRUE;

    Int_t nbinsx=h->GetNbinsX();
    Int_t nbinsy=h->GetNbinsY();

    Bool_t windowx=kFALSE;
    if(xmin!=0||xmax!=0) windowx=kTRUE;
    Bool_t windowy=kFALSE;
    if(ymin!=0||ymax!=0) windowy=kTRUE;

    TString name=h->GetName();
    name+="_gFit";

    TGraphErrors* g=0;
    g=new TGraphErrors();
    g->SetName(name.Data());

    Double_t max=h->GetMaximum();
    if(max==0) {
        delete g;
	return 0;
    }
    Int_t ctpoint=0;
    Double_t x,y,val,err;
    for(Int_t i=1;i<=nbinsx;i++){
        x=h->GetXaxis()->GetBinCenter(i);
        if(windowx) { if(x<xmin||x>xmax) continue;}
        for(Int_t j=1;j<=nbinsy;j++){
            y=h->GetYaxis()->GetBinCenter(j);
            if(windowy) { if(y<ymin||y>ymax) continue;}
            val=h->GetBinContent(i,j);
            if(val==0)continue;
            g->SetPoint(ctpoint,x,y);
            err=h->GetBinError(i,j);
            if(err==0) err=1e-10;
	    g->SetPointError(ctpoint,err,err);
            ctpoint++;
        }
    }
    if(!quiet)
    {
	cout<<"----------------------------------------------------"<<endl;
	cout<<"before cleaning:"<<endl;
	cout<<"----------------------------------------------------"<<endl;
    }
    g->Fit(f,opt.Data());
    if(usewindow)
    {
	if(!quiet)
	{
	    cout<<"----------------------------------------------------"<<endl;
	    cout<<"chi2   f : "<<f->GetChisquare()<<endl;
	    cout<<"----------------------------------------------------"<<endl;
	}
    }
    if(usewindow) delete g;

    if(usewindow)
    {
        g=new TGraphErrors();
        g->SetName(name.Data());
        Double_t ref;
        ctpoint=0;
        for(Int_t i=1;i<=nbinsx;i++){
            x=h->GetXaxis()->GetBinCenter(i);
            if(windowx) { if(x<xmin||x>xmax) continue;}
            for(Int_t j=1;j<=nbinsy;j++){
                y=h->GetYaxis()->GetBinCenter(j);
                if(windowy) { if(y<ymin||y>ymax) continue;}
                ref=f->Eval(x);
                if(y<ref-window||y>ref+window) { continue; }
                val=h->GetBinContent(i,j);
                if(val==0)continue;
                g->SetPoint(ctpoint,x,y);
		err=h->GetBinError(i,j);
                if(err==0) err=1e-10;
                g->SetPointError(ctpoint,0.,err);
                ctpoint++;
            }
        }
	if(!quiet)
	{
	    cout<<"----------------------------------------------------"<<endl;
	    cout<<"after cleaning:"<<endl;
	    cout<<"----------------------------------------------------"<<endl;
	}
	g->Fit(f,opt.Data());
        g->Print();

        //delete g;
    }
    if(clean)
    {
        Double_t ref;
        for(Int_t i=1;i<=nbinsx;i++)
        {
            x=h->GetXaxis()->GetBinCenter(i);
            for(Int_t j=1;j<=nbinsy;j++){
                y=h->GetYaxis()->GetBinCenter(j);
                ref=f->Eval(x);
                if(y<ref-window||y>ref+window)
                {
                    h->SetBinContent(i,j,0);
                    h->SetBinError(i,j,0);
                    continue;
                }
            }
        }
    }
    if(!quiet)
    {
	cout<<"----------------------------------------------------"<<endl;
	cout<<"Number of Points: "<<ctpoint<<endl;
	cout<<"window x : "<<xmin<<" , "<<xmax<<endl;
	cout<<"window y : "<<ymin<<" , "<<ymax<<endl;
	cout<<"window f : "<<window<<endl;
	cout<<"chi2   f : "<<f->GetChisquare()<<endl;
	cout<<"----------------------------------------------------"<<endl;
    }

    return g;
}
/*
TH1* HTool::removeEnds(const TH1* h,Int_t first,Int_t last,TString newname)
{
    // removes bins from start to first and from last to end
    // (first and last are kept). returns hist with name newname
    if(h==0)
    {
        cout<<"Error in HTool::removeEnds(): receiving zero pointer"<<endl;
        return 0;
    }
    if(newname.CompareTo("")==0)
    {
        newname =h->GetName();
        newname+="_remove";
    }
    if(last<0)last= h->GetNbinsX();
    TString classname=h->ClassName();
    TH1* hnew=0;
    if(classname.CompareTo("TH1I")==0)hnew=(TH1*)new TH1I(*(TH1I*)(h));
    if(classname.CompareTo("TH1S")==0)hnew=(TH1*)new TH1S(*(TH1S*)(h));
    if(classname.CompareTo("TH1F")==0)hnew=(TH1*)new TH1F(*(TH1F*)(h));
    if(classname.CompareTo("TH1D")==0)hnew=(TH1*)new TH1D(*(TH1D*)(h));
    if(classname.CompareTo("TProfile")==0)hnew=(TH1*)new TProfile(*(TProfile*)(h));
    if(hnew==0)
    {
        hnew->SetName(newname);
        hnew->SetBins(h->GetNbinsX()-(first-1)-last,h->GetXaxis()->GetBinLowEdge(first),h->GetXaxis()->GetBinUpEdge(last));
        for(Int_t i=1;i<=hnew->GetNbinsX();i++)
        {
            hnew->SetBinContent(i,h->GetBinContent(i,(first-1)+1));
            hnew->SetBinError(i,h->GetBinError(i,(first-1)+1));
        }
    }else cout<<"Error in HTool::removeEnds(): receiving zero pointer"<<endl;
    return hnew;
}*/
Int_t HTool::removeEnds(TH1* h,Int_t first,Int_t last)
{
    // removes bins from start to first and from last to end
    // (first and last are kept). returns hist with name newname

    cout<<"HTool::removeEnds():removing bins outside from first "<<first<<" last "<<last<<endl;
    if(h==0)
    {
        cout<<"Error in HTool::removeEnds(): receiving zero pointer"<<endl;
        return 0;
    }
    cout<<"test1"<<endl;
    TString newname =h->GetName();
    newname+="_remove";

    if(last<0)last= h->GetNbinsX();
    TString classname=h->ClassName();
    TH1* hnew=0;
    //if(classname.CompareTo("TH1I")==0)hnew=(TH1*)new TH1I(*(const TH1I*)(h));
    //if(classname.CompareTo("TH1S")==0)hnew=(TH1*)new TH1S(*(const TH1S*)(h));
    //if(classname.CompareTo("TH1F")==0)hnew=(TH1*)new TH1F(*(const TH1F*)(h));
    //if(classname.CompareTo("TH1D")==0)hnew=(TH1*)new TH1D(*(const TH1D*)(h));
    //if(classname.CompareTo("TProfile")==0)hnew=(TH1*)new TProfile(*(const TProfile*)(h));
    cout<<classname<<endl;
    const TH1D* hdummy=(TH1D*)h;
    hnew=new TH1D(*hdummy);
    hnew->Dump();


    cout<<"test2"<<endl;

    if(hnew==0)
    {
        hnew->SetName(newname);
        h->SetBins(hnew->GetNbinsX()-(first-1)-last,hnew->GetXaxis()->GetBinLowEdge(first),hnew->GetXaxis()->GetBinUpEdge(last));
        for(Int_t i=1;i<=h->GetNbinsX();i++)
        {
            h->SetBinContent(i,hnew->GetBinContent(i,(first-1)+1));
            h->SetBinError(i,hnew->GetBinError(i,(first-1)+1));
        }
    }else cout<<"Error in HTool::removeEnds(): receiving zero pointer"<<endl;
    cout<<"test3"<<endl;
    if(hnew) delete hnew;
    return 0;
}
Int_t HTool::findFilledRange(TH1* h,Int_t& first,Int_t& last)
{
    // find first bins and last filled bins

    if(h==0)
    {
        cout<<"Error in HTool::findFilledRange(): receiving zero pointer"<<endl;
        return 0;
    }
    Int_t nbins= h->GetNbinsX();
    Int_t f,l;
    f=0;
    l=0;
    first=0;
    last=nbins;
    for(Int_t i=1;i<=nbins;i++)
    {
       if(f==0 && h->GetBinContent(i)==0) first=i+1;
       else       f++;

       if(l==0 && h->GetBinContent(nbins-i)==0) last=nbins-i-1;
       else       l++;

       if(l>0&&f>0) return 0;
    }
    return 0;
}
void HTool::cleanHist(TH1* h,Double_t threshold,Double_t val)
{
    // clean hist if bincontent is below a certain value (threshold):
    // The bin content is replaced by val.

    if(h!=0)
    {
        TString classname=h->ClassName();
        Int_t type=0;

        if(classname.Contains("TH1")==1)type=1;
        if(classname.Contains("TH2")==1)type=2;

        Int_t binsX=0,binsY=0;

        binsX=h->GetNbinsX();
        if(type==2)binsY=h->GetNbinsY();

        Double_t bincontent;

        if(type==1)
        {
            for(Int_t x=0;x<=binsX;x++)
            {
                bincontent= h->GetBinContent(x+1);
		if(bincontent<threshold){
		    h->SetBinContent(x+1,val);
		}
	    }
        }
        if(type==2)
        {
            for(Int_t x=0;x<=binsX;x++)
            {
                for(Int_t y=0;y<=binsY;y++){
                    bincontent= h->GetBinContent(x+1,y+1);
		    if(bincontent<threshold){
			h->SetBinContent(x+1,y+1,val);
		    }
		}
            }
        }
    }
    else
    {
        cout<<"Warning: HTool::cleanHist : ZERO pointer for hist recieved!"<<endl;
    }
}
void HTool::resetHist(TH1* h,Float_t val,Float_t valerr)
{
    // reset hist with val for content and valerr for errors
    // (if this values not equal -99).


    if(h!=0)
    {
        TString classname=h->ClassName();
        Int_t type=0;

        if(classname.Contains("TH1")==1)type=1;
        if(classname.Contains("TH2")==1)type=2;

        Int_t binsX=0,binsY=0;

        binsX=h->GetNbinsX();
        if(type==2)binsY=h->GetNbinsY();

        if(type==1)
        {
            for(Int_t x=0;x<=binsX;x++)
            {
                if(val!=-99)   h->SetBinContent(x+1,val);
                if(valerr!=-99)h->SetBinError  (x+1,valerr);
            }
        }
        if(type==2)
        {
            for(Int_t x=0;x<=binsX;x++)
            {
                for(Int_t y=0;y<binsY;y++){
                    if(val!=-99)   h->SetBinContent(x+1,y+1,val);
                    if(valerr!=-99)h->SetBinError  (x+1,y+1,valerr);
                }
            }
        }
    }
    else
    {
        cout<<"Warning: HTool::resetHist : ZERO pointer for hist recieved!"<<endl;
    }
}
TH1* HTool::copyHist(TH1* h,TString name,Int_t val,Int_t valerr)
{
    // copy old hist into new one. You can decide to copy content of hist (val!=-99) and
    // errors (valerr!=-99). Combinations are possible.
    // if no new name is specified, _copy is appended to old name.

    TH1* hcp=0;
    if(h!=0)
    {
        TString classname=h->ClassName();
        TString myhistname="";
        Int_t type=0;

        if(classname.Contains("TH1")==1)type=1;
        if(classname.Contains("TH2")==1)type=2;
        if(classname.Contains("TProfile")==1)type=1;

        if(name.CompareTo("")==0)
        {   // if no new name, add _copy to old name
            myhistname=h->GetName();
            myhistname+=myhistname + "_copy";
        }
	else
	{
	   myhistname=name;
	}

        Int_t binsX=0,binsY=0;

        binsX=h->GetNbinsX();
        if(type==2)binsY=h->GetNbinsY();

        if(classname.Contains("TH1S")==1)hcp=new TH1S(myhistname.Data(),h->GetTitle(),binsX,h->GetXaxis()->GetXmin(),h->GetXaxis()->GetXmax());
        //if(classname.Contains("TH1I")==1)hcp=new TH1I(myhistname.Data(),h->GetTitle(),binsX,h->GetXaxis()->GetXmin(),h->GetXaxis()->GetXmax());
        if(classname.Contains("TH1F")==1)hcp=new TH1F(myhistname.Data(),h->GetTitle(),binsX,h->GetXaxis()->GetXmin(),h->GetXaxis()->GetXmax());
        if(classname.Contains("TH1D")==1)hcp=new TH1D(myhistname.Data(),h->GetTitle(),binsX,h->GetXaxis()->GetXmin(),h->GetXaxis()->GetXmax());
        if(classname.Contains("TProfile")==1)hcp=new TH1D(myhistname.Data(),h->GetTitle(),binsX,h->GetXaxis()->GetXmin(),h->GetXaxis()->GetXmax());

        if(classname.Contains("TH2S")==1)hcp=new TH2S(myhistname.Data(),h->GetTitle(),binsX,h->GetXaxis()->GetXmin(),h->GetXaxis()->GetXmax(),binsY,h->GetYaxis()->GetXmin(),h->GetYaxis()->GetXmax());
        //if(classname.Contains("TH2I")==1)hcp=new TH2I(myhistname.Data(),h->GetTitle(),binsX,h->GetXaxis()->GetXmin(),h->GetXaxis()->GetXmax(),binsY,h->GetYaxis()->GetXmin(),h->GetYaxis()->GetXmax());
        if(classname.Contains("TH2F")==1)hcp=new TH2F(myhistname.Data(),h->GetTitle(),binsX,h->GetXaxis()->GetXmin(),h->GetXaxis()->GetXmax(),binsY,h->GetYaxis()->GetXmin(),h->GetYaxis()->GetXmax());
        if(classname.Contains("TH2D")==1)hcp=new TH2D(myhistname.Data(),h->GetTitle(),binsX,h->GetXaxis()->GetXmin(),h->GetXaxis()->GetXmax(),binsY,h->GetYaxis()->GetXmin(),h->GetYaxis()->GetXmax());

        hcp->SetXTitle(h->GetXaxis()->GetTitle());
        hcp->SetYTitle(h->GetYaxis()->GetTitle());
        hcp->SetOption(hcp->GetOption());

        hcp->SetFillColor(hcp->GetFillColor());
        hcp->SetFillStyle(hcp->GetFillStyle());

        hcp->SetLineColor(hcp->GetLineColor());
        hcp->SetLineStyle(hcp->GetLineStyle());
        hcp->SetLineWidth(hcp->GetLineWidth());

        hcp->SetMarkerColor(hcp->GetMarkerColor());
        hcp->SetMarkerStyle(hcp->GetMarkerStyle());
        hcp->SetMarkerSize(hcp->GetMarkerSize());

        if(type==2)
        {
            hcp->SetZTitle(h->GetZaxis()->GetTitle());
        }

        if(type==1)
        {
            for(Int_t x=0;x<=binsX;x++)
            {
                if(val!=-99)   hcp->SetBinContent(x+1,h->GetBinContent(x+1));
                if(valerr!=-99)hcp->SetBinError  (x+1,h->GetBinError(x+1));
            }
        }
        if(type==2)
        {
            for(Int_t x=0;x<=binsX;x++)
            {
                for(Int_t y=0;y<=binsY;y++){
                    if(val!=-99)   hcp->SetBinContent(x+1,y+1,h->GetBinContent(x+1,y+1));
                    if(valerr!=-99)hcp->SetBinError  (x+1,y+1,h->GetBinError(x+1,y+1));
                }
            }
        }
    }
    else
    {
        cout<<"Warning: HTool::copyHist : ZERO pointer for hist recieved!"<<endl;
    }
    return hcp;
}
TH1* HTool::copyHistRange(TH1* h,TString name,Int_t val,Int_t valerr,Int_t start,Int_t end)
{
  // copy old hist into new one. You can decide to copy content of hist (val!=-99) and
  // errors (valerr!=-99). Combinations are possible.
  // if no new name is specified, _copy is appended to old name.
  // Copy is done in range from start to end. If end = -99, Full histogram is copied.

  TH1* hcp=0;
  if(h!=0)
    {
      TString classname=h->ClassName();
      TString myhistname="";
      Int_t type=0;

      if(classname.Contains("TH1")==1)type=1;
      if(classname.Contains("TH2")==1)type=2;
      if(classname.Contains("TProfile")==1)type=1;

      if(name.CompareTo("")==0)
        {   // if no new name, add _copy to old name
	  myhistname=h->GetName();
	  myhistname+=myhistname + "_copy";
        }
      else
	{
	  myhistname = name;
	}

      Int_t binsX=0,binsY=0;

      if(end!=-99)
	{
	  binsX = end - start;
	}
      else
	{
	  binsX = h->GetNbinsX() - start;
	  end = h->GetNbinsX();
	}
      if(type==2)binsY=h->GetNbinsY();

      if(classname.Contains("TH1S")==1)hcp=new TH1S(myhistname.Data(),h->GetTitle(),binsX,h->GetXaxis()->GetBinLowEdge(start),h->GetXaxis()->GetBinLowEdge(end));
      //if(classname.Contains("TH1I")==1)hcp=new TH1I(myhistname.Data(),h->GetTitle(),binsX,h->GetXaxis()->GetXmin(),h->GetXaxis()->GetXmax());
      if(classname.Contains("TH1F")==1)hcp=new TH1F(myhistname.Data(),h->GetTitle(),binsX,h->GetXaxis()->GetBinLowEdge(start),h->GetXaxis()->GetBinLowEdge(end));
      if(classname.Contains("TH1D")==1)hcp=new TH1D(myhistname.Data(),h->GetTitle(),binsX,h->GetXaxis()->GetBinLowEdge(start),h->GetXaxis()->GetBinLowEdge(end));
      if(classname.Contains("TProfile")==1)hcp=new TH1D(myhistname.Data(),h->GetTitle(),binsX,h->GetXaxis()->GetBinLowEdge(start),h->GetXaxis()->GetBinLowEdge(end));

      if(classname.Contains("TH2S")==1)hcp=new TH2S(myhistname.Data(),h->GetTitle(),binsX,h->GetXaxis()->GetXmin(),h->GetXaxis()->GetBinLowEdge(start),binsY,h->GetYaxis()->GetBinLowEdge(end),h->GetYaxis()->GetXmax());
      //if(classname.Contains("TH2I")==1)hcp=new TH2I(myhistname.Data(),h->GetTitle(),binsX,h->GetXaxis()->GetXmin(),h->GetXaxis()->GetXmax(),binsY,h->GetYaxis()->GetXmin(),h->GetYaxis()->GetXmax());
      if(classname.Contains("TH2F")==1)hcp=new TH2F(myhistname.Data(),h->GetTitle(),binsX,h->GetXaxis()->GetXmin(),h->GetXaxis()->GetBinLowEdge(start),binsY,h->GetYaxis()->GetBinLowEdge(end),h->GetYaxis()->GetXmax());
      if(classname.Contains("TH2D")==1)hcp=new TH2D(myhistname.Data(),h->GetTitle(),binsX,h->GetXaxis()->GetBinLowEdge(start),h->GetXaxis()->GetBinLowEdge(end),binsY,h->GetYaxis()->GetXmin(),h->GetYaxis()->GetXmax());

      hcp->SetXTitle(h->GetXaxis()->GetTitle());
      hcp->SetYTitle(h->GetYaxis()->GetTitle());
      hcp->SetOption(hcp->GetOption());

      hcp->SetFillColor(hcp->GetFillColor());
      hcp->SetFillStyle(hcp->GetFillStyle());

      hcp->SetLineColor(hcp->GetLineColor());
      hcp->SetLineStyle(hcp->GetLineStyle());
      hcp->SetLineWidth(hcp->GetLineWidth());

      hcp->SetMarkerColor(hcp->GetMarkerColor());
      hcp->SetMarkerStyle(hcp->GetMarkerStyle());
      hcp->SetMarkerSize(hcp->GetMarkerSize());

      if(type==2)
        {
	  hcp->SetZTitle(h->GetZaxis()->GetTitle());
        }

      if(type==1)
        {
	  for(Int_t x=start;x<=end;x++)
            {
	      if(val!=-99)   hcp->SetBinContent(x+1,h->GetBinContent(x+1));
	      if(valerr!=-99)hcp->SetBinError  (x+1,h->GetBinError(x+1));
            }
        }
      if(type==2)
        {
	  for(Int_t x=start;x<=end;x++)
            {
	      for(Int_t y=0;y<=binsY;y++){
		if(val!=-99)   hcp->SetBinContent(x+1,y+1,h->GetBinContent(x+1,y+1));
		if(valerr!=-99)hcp->SetBinError  (x+1,y+1,h->GetBinError(x+1,y+1));
	      }
            }
        }
    }
  else
    {
      cout<<"Warning: HTool::copyHist : ZERO pointer for hist recieved!"<<endl;
    }
  return hcp;
}
TF1* HTool::cleanHistBelowLine(TH2* h,TF1* f,TString axis,Int_t firstbin,Int_t lastbin,Int_t cut,TString opt,TString suffix,
                               TString optline,Float_t windowfunc)
{
    // beforms slices HTool::operation and fits the result with a line
    // Resets the bins blow line equation - windowfunc to 0.

    TObjArray* array =HTool::slices(h,f,axis,firstbin,lastbin,cut,opt,suffix,8,2,0.7);
    TH1D* h1=(TH1D*)array->At(1);

    TF1* fit=new TF1("tempfit",optline.Data());
    h1->Fit(fit,"QN");


    Int_t nbinsX=h->GetNbinsX();
    Int_t nbinsY=h->GetNbinsY();
    Float_t lowedgeY;
    Float_t myY;
    for(Int_t i=0;i<=nbinsX;i++)
    {
        lowedgeY=fit->Eval(h->GetBinCenter(i+1))-windowfunc;
        for(Int_t j=0;j<=nbinsY;j++)
        {
           myY=h->GetYaxis()->GetBinCenter(j+1);
           if(myY<lowedgeY)
           {
               h->SetBinContent(i+1,j+1,0);
               h->SetBinError(i+1,j+1,0);
           }
        }
    }
    array->Delete();
    delete array;
    return fit;
}

TF1* HTool::cleanHistArroundLine(TH2* h,TF1* f,TString axis,Int_t firstbin,Int_t lastbin,Int_t cut,TString opt,TString suffix,
                               TString optline,Float_t windowfunc,Float_t windowfunc2)
{
    // beforms slices HTool::operation and fits the result with a line
    // Resets the bins arround line equation +- windowfunc to 0.

    TObjArray* array =HTool::slices(h,f,axis,firstbin,lastbin,cut,opt,suffix,8,2,0.7);
    TH1D* h1=(TH1D*)array->At(1);

    TF1* fit=new TF1("tempfit",optline.Data());
    h1->Fit(fit,"QN");


    Int_t nbinsX=h->GetNbinsX();
    Int_t nbinsY=h->GetNbinsY();
    Float_t lowedgeY;
    Float_t upedgeY;
    Float_t myY;
    for(Int_t i=0;i<=nbinsX;i++)
    {
        lowedgeY=fit->Eval(h->GetBinCenter(i+1))-windowfunc;
        upedgeY =fit->Eval(h->GetBinCenter(i+1))+windowfunc2;
        for(Int_t j=0;j<=nbinsY;j++)
        {
           myY=h->GetYaxis()->GetBinCenter(j+1);
           if(myY<lowedgeY||myY>upedgeY)
           {
               h->SetBinContent(i+1,j+1,0);
               h->SetBinError(i+1,j+1,0);
           }
        }
    }
    array->Delete();
    delete array;
    return fit;
}
Bool_t  HTool::cleanHistCutG(TH2* h,TCutG* cut)
{
    // puts all bins outside the cut to 0.
    if(!h || !cut)
    {
        cout<<"HTool::cleanHistCutG():ZERO POINTER!"<<endl;
        return kFALSE;
    }

    Int_t nBinsX=h->GetNbinsX();
    Int_t nBinsY=h->GetNbinsY();
    Float_t x,y;

    for(Int_t i=0;i<=nBinsX;i++){
        x=h->GetXaxis()->GetBinCenter(i+1);
        for(Int_t j=0;j<=nBinsY;j++){
            y=h->GetYaxis()->GetBinCenter(j+1);

	    if(cut->IsInside(x,y)==0)
	    {
		h->SetBinContent(i+1,j+1,0);
	    }
	}
    }
    return kTRUE;
}

void HTool::setHistErrors(TH1* h,TH1* h2)
{
    // sets bincontent of hist h2 as errors of h

    if(h!=0&&h2!=0)
    {
        TString classname=h->ClassName();
        Int_t type=0;

        if(classname.Contains("TH1")==1)type=1;
        if(classname.Contains("TH2")==1)type=2;

        Int_t binsX=0,binsY=0;

        binsX=h->GetNbinsX();
        if(type==2)binsY=h->GetNbinsY();

        Double_t bincontent;

        if(type==1)
        {
            for(Int_t x=0;x<=binsX;x++)
            {
                bincontent= h2->GetBinContent(x+1);
                h->SetBinError(x+1,bincontent);
            }
        }
        if(type==2)
        {
            for(Int_t x=0;x<=binsX;x++)
            {
                for(Int_t y=0;y<=binsY;y++){
                    bincontent= h2->GetBinContent(x+1,y+1);
                    h->SetBinError(x+1,y+1,bincontent);
                }
            }
        }
    }
    else
    {
        cout<<"Warning: HTool::cleanHist : ZERO pointer for hist recieved!"<<endl;
    }
}
Double_t HTool::getMaxHistArray(TH1** h,Int_t size,Int_t& index)
{
    // Finds maximum of all hists in TH1* array of size size.
    // return max and the index of the hist inside the array
    // which has the highest max.
    if(size==0)
    {
        cout<<"HTool::getMaxHistArray()::Size of Array is ZERO!"<<endl;
        return 0;
    }
    Double_t max[size];

    for(Int_t i=0;i<size;i++){
       max[i]=h[i]->GetMaximum();
    }
    Int_t maxindex= TMath::LocMax(size,max);
    index=maxindex;
    return max[maxindex];
}
Double_t HTool::getMinHistArray(TH1** h,Int_t size,Int_t& index)
{
    // Finds minimum of all hists in TH1* array of size size.
    // return min and the index of the hist inside the array
    // which has the lowest min.
    if(size==0)
    {
        cout<<"HTool::getMaxHistArray()::Size of Array is ZERO!"<<endl;
        return 0;
    }
    Double_t min[size];

    for(Int_t i=0;i<size;i++){
       min[i]=h[i]->GetMinimum();
    }
    Int_t minindex= TMath::LocMin(size,min);
    index=minindex;
    return min[minindex];
}
TH2* HTool::reBin(TH2* h2,Int_t groupX,Int_t groupY,TString newname)
{
    // Rebins h2 with groups of groupX and groupY. The new created
    // hist with name newname (if specified) or oldname_rebin will
    // be returned.

    TH2* result=0;
    if(h2==0) return result;
    TString myname="";
    if(newname.CompareTo("")==0)
    { // default
	myname=h2->GetName();
	myname+="_rebin";
    } else
    {
	myname=newname;
    }

    if(groupX!=0&&groupY!=0)
    {
	result=(TH2*)h2->Clone(myname.Data());

        Int_t nbinsx=h2->GetNbinsX();
        Int_t nbinsy=h2->GetNbinsY();
        Float_t xmin=h2->GetXaxis()->GetXmin();
        Float_t xmax=h2->GetXaxis()->GetXmax();
        Float_t ymin=h2->GetYaxis()->GetXmin();
        Float_t ymax=h2->GetYaxis()->GetXmax();

	Int_t nbinsX=0;
        Int_t nbinsY=0;

	if(nbinsx%groupX!=0)
	{
	    nbinsX=(Int_t)(nbinsx/groupX)+1;
	}
	else
	{
           nbinsX=nbinsx/groupX;
	}
	if(nbinsy%groupY!=0)
	{
	    nbinsY=(Int_t)(nbinsy/groupY)+1;
	}
	else
	{
           nbinsY=nbinsy/groupY;
	}

	result->SetBins(nbinsX,xmin,xmax,nbinsY,ymin,ymax);
	result->Reset();
    }
    else
    {
	cout<<"HTool::reBin(): groupX or groupY =0!!!"<<endl;
        return 0;
    }
    cout<<"Rebin:"<<endl;
	cout<<"from :"<<h2->GetNbinsX()<<" "
        <<h2->GetXaxis()->GetXmin()<<" "
        <<h2->GetXaxis()->GetXmax()<<" "
        <<h2->GetNbinsY()<<" "
        <<h2->GetYaxis()->GetXmin()<<" "
        <<h2->GetYaxis()->GetXmax()<<endl;
    cout<<"to   :"<<result->GetNbinsX()<<" "
        <<result->GetXaxis()->GetXmin()<<" "
        <<result->GetXaxis()->GetXmax()<<" "
        <<result->GetNbinsY()<<" "
        <<result->GetYaxis()->GetXmin()<<" "
        <<result->GetYaxis()->GetXmax()<<endl;

    Int_t nBinsX=h2->GetNbinsX();
    Int_t nBinsY=h2->GetNbinsY();
    Float_t x,y;


    for(Int_t i=1;i<=nBinsX;i++){
	for(Int_t j=1;j<=nBinsY;j++){
            x=h2->GetXaxis()->GetBinCenter(i);
            y=h2->GetYaxis()->GetBinCenter(j);

	    result->Fill(x,y,h2->GetBinContent(i,j));

	}
    }
    return result;
}
TH2* HTool::exchangeXY(TH2* h2,TString newname)
{
    // creates new hist with name newname (if specified)
    // or oldname_exchangeXY with exchanged
    // x- and y-axis

    TH2* result=0;
    if(h2==0) return result;
    TString myname="";
    if(newname.CompareTo("")==0)
    { // default
	myname=h2->GetName();
	myname+="_exchangeXY";
    } else
    {
	myname=newname;
    }
    Int_t nbinsx=h2->GetNbinsX();
    Int_t nbinsy=h2->GetNbinsY();
    Float_t xmin=h2->GetXaxis()->GetXmin();
    Float_t xmax=h2->GetXaxis()->GetXmax();
    Float_t ymin=h2->GetYaxis()->GetXmin();
    Float_t ymax=h2->GetYaxis()->GetXmax();

    result=(TH2*)h2->Clone(myname.Data());
    result->SetBins(nbinsy,ymin,ymax,nbinsx,xmin,xmax);

    for(Int_t i=0;i<=nbinsx;i++){
	for(Int_t j=0;j<=nbinsy;j++){

            result->SetBinContent(j+1,i+1,h2->GetBinContent(i+1,j+1));
	    result->SetBinError  (j+1,i+1,h2->GetBinError(i+1,j+1));
        }
    }
    return result;

}
Bool_t HTool::flipAxis(TH2* h2,TString opt)
{
    // flips x or y axis or both depending on opt (x,y,xy)

    if(h2==0) return kFALSE;
    opt.ToLower();
    Int_t ax=0;
    if(opt.CompareTo("x")==0)
    {
        ax=0;
    }
    else if(opt.CompareTo("y")==0)
    {
      ax=1;
    }
    else if(opt.CompareTo("xy")==0)
    {
      ax=2;
    }
    else
    {
        cout<<"HTool::flipAxis():Unknown opt "<<opt.Data()<<"!"<<endl;
        return kFALSE;
    }
    Int_t nbinsx=h2->GetNbinsX();
    Int_t nbinsy=h2->GetNbinsY();
    Float_t xmin=h2->GetXaxis()->GetXmin();
    Float_t xmax=h2->GetXaxis()->GetXmax();
    Float_t ymin=h2->GetYaxis()->GetXmin();
    Float_t ymax=h2->GetYaxis()->GetXmax();

    TH2* hclone=(TH2*)h2->Clone("temp_clone");

    if(ax==0)h2->SetBins(nbinsx,-xmax,-xmin,nbinsy, ymin, ymax);
    if(ax==1)h2->SetBins(nbinsx, xmin, xmax,nbinsy,-ymax,-ymin);
    if(ax==2)h2->SetBins(nbinsx,-xmax,-xmin,nbinsy,-ymax,-ymin);

    for(Int_t i=1;i<=nbinsx;i++){
        for(Int_t j=1;j<=nbinsy;j++){

            if(ax==0||ax==2)
            {   // flip x
                h2->SetBinContent(i,j,hclone->GetBinContent(nbinsx+1-i,j));
                h2->SetBinError  (i,j,hclone->GetBinError  (nbinsx+1-i,j));
            }
            if(ax==1||ax==2)
            {   // flip y
                h2->SetBinContent(i,j,hclone->GetBinContent(i,nbinsy+1-j));
                h2->SetBinError  (i,j,hclone->GetBinError  (i,nbinsy+1-j));
            }
        }
    }
    delete hclone;
    return kTRUE;
}
TGraph* HTool::histToGraph(TH1* h,TString newname,Bool_t exchange,Int_t markerstyle,Int_t markercolor,Float_t markersize)
{
    // Creates a TGraph from hist h. The axis can be excganged if
    // exchange=kTRUE; The Graph does not take empty bins into account.

    TGraph* result=0;
    if(h==0) return result;
    TString myname="";
    if(newname.CompareTo("")==0)
    { // default
	myname=h->GetName();
	myname+="_graph";
    } else
    {
	myname=newname;
    }

    Int_t nBinsX=h->GetNbinsX();
    Double_t x,y;

    result=new TGraph();
    result->SetName(myname.Data());
    result->SetLineColor(1);
    result->SetMarkerStyle(markerstyle);
    result->SetMarkerColor(markercolor);
    result->SetMarkerSize (markersize);

    for(Int_t i=0;i<nBinsX;i++)
    {
	x=h->GetXaxis()->GetBinCenter(i+1);
	y=h->GetBinContent(i+1);

	if(y!=0)
	{
	    if(!exchange)result->SetPoint(i,x,y);
            else         result->SetPoint(i,y,x);
	}
	else
	{
	    result->SetPoint(i,0.,0.);
	}
    }

    for(Int_t i=0;i<nBinsX;i++)
    {
	for(Int_t j=0;j<nBinsX;j++)
	{
	    if(!exchange)result->GetPoint(j,x,y);
            else         result->GetPoint(j,y,x);

	    if(y==0||!finite(y))
	    {
		result->RemovePoint(j);
		break;
	    }
	}
    }
    return result;
}
TGraphErrors* HTool::histToGraphErrors(TH1* h,TString newname,Bool_t exchange,Int_t markerstyle,Int_t markercolor,Float_t markersize)
{
    // Creates a TGraphErrors from hist h. The axis can be excganged if
    // exchange=kTRUE; The Graph does not take empty bins into account.

    TGraphErrors* result=0;
    if(h==0) return result;
    TString myname="";
    if(newname.CompareTo("")==0)
    { // default
	myname=h->GetName();
	myname+="_graph";
    } else
    {
	myname=newname;
    }

    Int_t nBinsX=h->GetNbinsX();
    Double_t x,y;
    Double_t xE,yE;

    result=new TGraphErrors();
    result->SetName(myname.Data());
    result->SetLineColor(1);
    result->SetMarkerStyle(markerstyle);
    result->SetMarkerColor(markercolor);
    result->SetMarkerSize (markersize);

    for(Int_t i=0;i<nBinsX;i++)
    {
	x =h->GetXaxis()->GetBinCenter(i+1);
	y =h->GetBinContent(i+1);
        xE=h->GetXaxis()->GetBinWidth(i+1);
        yE=h->GetBinError(i+1);

        if(y!=0)
	{
            if(!exchange)
            {
                result->SetPoint(i,x,y);
                result->SetPointError(i,xE,yE);
            }
            else
            {
                result->SetPoint(i,y,x);
                result->SetPointError(i,yE,xE);
            }
        }
	else
	{
	    result->SetPoint(i,0.,0.);
	}
    }

    for(Int_t i=0;i<nBinsX;i++)
    {
	for(Int_t j=0;j<nBinsX;j++)
	{
            if(!exchange)
            {
                result->GetPoint(j,x,y);
            }
            else
            {
                result->GetPoint(j,y,x);
            }
	    if(y==0||!finite(y))
	    {
		result->RemovePoint(j);
		break;
	    }
	}
    }
    return result;
}
TGraph* HTool::hist2DToGraph(TH2* h,Float_t xmin,Float_t xmax,
			     Float_t ymin,Float_t ymax
			    )
{
    // Puts h into TGraph .
    // With xmin/xmax/ymin,ymax a region can be selected.

    if(h==0) return 0;

    Int_t nbinsx=h->GetNbinsX();
    Int_t nbinsy=h->GetNbinsY();

    Bool_t windowx=kFALSE;
    if(xmin!=0||xmax!=0) windowx=kTRUE;
    Bool_t windowy=kFALSE;
    if(ymin!=0||ymax!=0) windowy=kTRUE;

    TString name=h->GetName();
    name+="_gTH2";

    TGraph* g=0;
    g=new TGraph();
    g->SetName(name.Data());

    Double_t max=h->GetMaximum();
    if(max==0) {
        delete g;
	return 0;
    }
    Double_t x,y,val;
    Int_t ctpoint=0;
    for(Int_t i=1;i<=nbinsx;i++){
        x=h->GetXaxis()->GetBinCenter(i);
        if(windowx) { if(x<xmin||x>xmax) continue;}
        for(Int_t j=1;j<=nbinsy;j++){
            y=h->GetYaxis()->GetBinCenter(j);
            if(windowy) { if(y<ymin||y>ymax) continue;}
            val=h->GetBinContent(i,j);
            if(val==0)continue;
	    g->SetPoint(ctpoint,x,y);
	    ctpoint++;
        }
    }

    return g;
}
TGraphErrors* HTool::hist2DToGraphErrors(TH2* h,
					 Float_t xmin,Float_t xmax,
					 Float_t ymin,Float_t ymax
					)
{
    // Puts h into TGraphErrors .
    // With xmin/xmax/ymin,ymax a region can be selected.

    if(h==0) return 0;

    Int_t nbinsx=h->GetNbinsX();
    Int_t nbinsy=h->GetNbinsY();

    Bool_t windowx=kFALSE;
    if(xmin!=0||xmax!=0) windowx=kTRUE;
    Bool_t windowy=kFALSE;
    if(ymin!=0||ymax!=0) windowy=kTRUE;

    TString name=h->GetName();
    name+="_gTH2";

    TGraphErrors* g=0;
    g=new TGraphErrors();
    g->SetName(name.Data());

    Double_t max=h->GetMaximum();
    if(max==0) {
        delete g;
	return 0;
    }
    Double_t x,y,val,err;
    Int_t ctpoint=0;
    for(Int_t i=1;i<=nbinsx;i++){
        x=h->GetXaxis()->GetBinCenter(i);
        if(windowx) { if(x<xmin||x>xmax) continue;}
        for(Int_t j=1;j<=nbinsy;j++){
            y=h->GetYaxis()->GetBinCenter(j);
            if(windowy) { if(y<ymin||y>ymax) continue;}
            val=h->GetBinContent(i,j);
            if(val==0)continue;
            g->SetPoint(ctpoint,x,y);
            err=h->GetBinError(i,j);
	    g->SetPointError(ctpoint,err,err);
	    ctpoint++;
        }
    }

    return g;
}


Bool_t HTool::shiftHistByBin(TH1* h,Int_t shiftbin)
{
    // shifts hist content by shiftbin bins
    if(!h) return kFALSE;
    Int_t nbin=h->GetNbinsX();
    TH1* hclone=(TH1*)h->Clone("clone");
    for(Int_t i=1;i<=nbin;i++)
    {
        if((i+shiftbin)>1&&(i+shiftbin)<=nbin)
        {
            h->SetBinContent(i+shiftbin,hclone->GetBinContent(i));
        }
    }
    delete hclone;
    return kTRUE;
}
Bool_t HTool::shiftHist(TH1* h,Float_t shift)
{
    // shifts hist content by value shift
    if(!h) return kFALSE;
    Int_t nbin=h->GetNbinsX();
    TH1* hclone=(TH1*)h->Clone("clone");
    Float_t xmin=h->GetXaxis()->GetXmin();
    Float_t xmax=h->GetXaxis()->GetXmax();

    for(Int_t i=1;i<=nbin;i++)
    {
        if((i+shift)>=xmin&&(i+shift)<=xmax)
        {
            h->Fill(hclone->GetXaxis()->GetBinCenter(i),hclone->GetBinContent(i));
        }
    }
    delete hclone;
    return kTRUE;
}
Int_t HTool::normalize_max(TH2* ht,TString axis)
{
    // Project to axis and and calc scaling factor for the bin
    // to maximum of total hist max_total/max_proj. The bins of
    // the hist are scaled by the factor. The result is 2D hist
    // where each bin Projection has the same maximum height.
    // The opration is performed on the original Hist!
    // arguments:
    // TH2* ht        :pointer to hist which should be normalized
    // TString axis   :="x" , or "y" depending on the projection
    // return values:
    // -1 : In the case of zero pointer input or unknown axis argument
    //  0 : If opreation has been succesful
    //  3 : TH2 max is 0 (most probably empty).

    if(!ht)
    {
	cout<<"HTool::normalize_max(TH2*,TString axis):: ZERO pointer!"<<endl;

	return -1;
    }

    Double_t max = ht->GetMaximum();
    if(max==0) return 3;

    Int_t nbinsx=ht->GetNbinsX();
    Int_t nbinsy=ht->GetNbinsY();

    axis.ToLower();
    if(axis.CompareTo("y")==0)
    {
	for(Int_t i=0; i<nbinsy;i++)
	{
	    TH1D* hproj=ht->ProjectionX("proj",i+1,i+1,"");
	    hproj->SetDirectory(0);

	    Double_t max_y=hproj->GetMaximum();
	    if(max_y==0) continue;
	    Double_t scale=max/max_y;

	    for(Int_t j=0;j<nbinsx;j++)
	    {
		Double_t content=ht->GetBinContent(j+1,i+1);
		ht->SetBinContent(j+1,i+1,scale*content);
	    }

	    delete hproj;
	}
	return 0;
    }
    else if(axis.CompareTo("x")==0)
    {
	for(Int_t i=0; i<nbinsx;i++)
	{
	    TH1D* hproj=ht->ProjectionY("proj",i+1,i+1,"");
	    hproj->SetDirectory(0);

	    Double_t max_y=hproj->GetMaximum();
	    if(max_y==0) continue;
	    Double_t scale=max/max_y;

	    for(Int_t j=0;j<nbinsy;j++)
	    {
		Double_t content=ht->GetBinContent(i+1,j+1);
		ht->SetBinContent(i+1,j+1,scale*content);
	    }

	    delete hproj;
	}
	return 0;
    }
    else
    {
	cout<<"HTool::normalize_max(TH2*,TString axis):: unknown option for axis :"<<axis.Data()<<endl;
	return -1;
    }
}

//---------------------------graphs--------------------------------

Bool_t HTool::exchangeXYGraph(TGraph* g)
{
    // exchanges x-values and y-values.
    if(g==0) return kFALSE;
    Int_t nbin=g->GetN();
    Double_t x,y;
    for(Int_t i=0;i<nbin;i++)
    {
        g->GetPoint(i,x,y);
        g->SetPoint(i,x,y);
    }

    return kTRUE;
}
Bool_t HTool::scaleGraph(TGraph* g,Double_t xscale,Double_t yscale)
{
    // scales x-values and y-values with  xscale,yscale.
    if(g==0) return kFALSE;
    Int_t nbin=g->GetN();
    Double_t x,y;
    for(Int_t i=0;i<nbin;i++)
    {
        g->GetPoint(i,x,y);
        g->SetPoint(i,x*xscale,y*yscale);
    }
    return kTRUE;
}
Bool_t HTool::shiftGraph(TGraph* g,Double_t xshift,Double_t yshift)
{
    // add shift to x-values and y-values
    if(g==0) return kFALSE;
    Int_t nbin=g->GetN();
    Double_t x,y;
    for(Int_t i=0;i<nbin;i++)
    {
        g->GetPoint(i,x,y);
        g->SetPoint(i,x+xshift,y+yshift);
    }
    return kTRUE;
}

//---------------------strings--------------------------------------

TString*  HTool::parseString(TString options,Int_t& size,TString separator,Bool_t tolower)
{
    // loops over TString options and find substrings separated by separator
    // and puts them to an array of TStrings. size will hold the size of this
    // array and the pointer to the array is returned. If tolower is kTRUE options
    // will be made toLower.
    if(tolower)options.ToLower();
    options.ReplaceAll(" ","");
    Ssiz_t len=options.Length();
    TString* sarray=0;
    Int_t count=0;
    if(len!=0)
    {
        TObjArray array;
        char* mystring=(char*)options.Data();
        Char_t* buffer;
        TObjString* stemp;

        while(1) // find all token in option string and put them to a list
        {
            if(count==0)
            {
                buffer=strtok(mystring,separator.Data());
                stemp=new TObjString(buffer);
                array.Add(stemp);
            }

            if(!(buffer=strtok(NULL,separator.Data())))break;
            stemp=new TObjString(buffer);
            array.Add(stemp);

            count++;
        }
        sarray= new TString [count+1];

        for(Int_t i=0;i<count+1;i++)
        {
          sarray[i]= ((TObjString*)(array.At(i)))->GetString();
        }
        array.Delete();
    }

    size=count+1;
    return sarray;
}
Bool_t  HTool::findString(TString* classes,Int_t size,TString name)
{
    // searches name in TString array classes with size size.
    // return kTRUE if the string is found.
    for(Int_t i=0;i<size;i++)
    {
        if(classes[i].CompareTo(name)==0)return kTRUE;
    }
    return kFALSE;
}

//---------------------------tree compare---------------------------

Bool_t  HTool::readHistsDescription(TString file,TObjArray* myhists,TString* classes,Int_t sizeclass)
{
    // reads the hist descriptions from ascii file file.
    // Creates the hists found in TString array classes
    // with size size and adds them to TObjArray myhists.

    FILE* ascii=fopen(file.Data(),"r");
    if(!ascii)
    {
          cout<<"reading hist description: Specified file "<<file.Data()<<" does not exist!"<<endl;
          return kFALSE;
    }
    else
    {
        TString buffer=0;
        Char_t line[400];
        Int_t nbins;
        Float_t xmin,xmax;
        TString name;
        Int_t size;

        while(1)
        {

            if(feof(ascii)) break;
            fgets(line, sizeof(line), ascii);
            buffer=line;
            if(buffer.Contains("#") !=0)continue;
            if(buffer.Contains("//")!=0)continue;

            TString* ar=HTool::parseString(buffer,size,",",kFALSE);
            if(size==4)
            {
                name=ar[0];
                sscanf(ar[1].Data(),"%i",&nbins);
                sscanf(ar[2].Data(),"%f",&xmin);
                sscanf(ar[3].Data(),"%f",&xmax);
                if(classes!=0)
                {
                    if(HTool::findString(&classes[0],sizeclass,name))
                    {
                        myhists->Add(new TH1F(name.Data(),name.Data(),nbins,xmin,xmax));
                    }
                }
                else
                {
                    myhists->Add(new TH1F(name.Data(),name.Data(),nbins,xmin,xmax));
                }
            }else {
                cout<<"readHistsDescription(): Wrong number of arguments!"<<endl;
            }

        }
        fclose(ascii);
    }
    return kTRUE;
}
Bool_t HTool::makeHists(TString infile,Int_t evs,TString histdescriptionfile,TString listofClasses)
{
    // makeHists(TString infile,Int_t evs,TString histdescriptionfile,TString listofClasses)
    // Takes all categories in infile and generates generic histograms of all datamembers.
    // T->Draw() is used to fill the hists. evs is the number of entries to loop over. if evs=0
    // the full size of the tree will be used.
    // INPUT:
    // histdescriptionfile is the file with description of the histograms to fill (ascii).
    // if histdescriptionfile=="" histograms will be filled generic and a histdescription file
    // histDescription.txt will be generated in the working directory.
    // listofClasses contains a list of Classes which should be taken into account, all other
    // classes will be ignored. if listofClasses=="" all classes in infile will be taken.
    // OUTPUT:
    // I.   a root file treeHists.root will be generated which contains all histograms of the
    //      selected variables
    // II.  if histdescriptionfile has been empty an ascii file treeHistDescription.txt will
    //      be generated in the working directory.
    // III. an ascii file treeHistResults.txt with the analysis results of the hists will be created,
    //      which can be used for diff of different files.

    if (infile.CompareTo("")==0)
    {
        cout<<"No input file speciefied!"<<endl;
        return kFALSE;
    }
    if (evs==0)
    {
        return kFALSE;
    }
    Bool_t useDescription=kFALSE;
    if (histdescriptionfile.CompareTo("")!=0)
    {
        useDescription=kTRUE;
    }
    Bool_t useClassList=kFALSE;
    if (listofClasses.CompareTo("")!=0)
    {
        useClassList=kTRUE;
    }

    Int_t numberclasses=0;
    TString* myclassNames=0;

    if(useClassList)
    {
        myclassNames=HTool::parseString(listofClasses,numberclasses,",",kFALSE);
    }

    TFile* out=new TFile("treeHists.root","RECREATE");
    TCanvas* result=new TCanvas("result","result",1000,800);
    result->cd();

    TFile* in=new TFile(infile.Data(),"READ");
    in->cd();
    Int_t events=evs;
    TTree* T=(TTree*)in->Get("T");

    if (evs==0)
    {
        events=(Int_t)T->GetEntries();
    }

    TObjArray* classarray=new TObjArray();
    TString keyname;
    TString keyclassname;

    //------------------Looping directories to find all categories in file--------------------------
    // has to done only if no histdescription is used!!

    TList* keylist=0;
    Int_t sizekey =0;
    Int_t counter=0;

    if(!useDescription)
    {

        keylist=gDirectory->GetListOfKeys();
        sizekey =keylist->LastIndex();
        counter=0;
        for(Int_t i=0;i<sizekey+1;i++)
        {
            TKey* key=(TKey*)keylist->At(i);
            keyname     = key->GetName();
            keyclassname= key->GetClassName();

            if(keyclassname.CompareTo("TDirectory")==0)
            {
                cout<<keyname.Data()<<" "<<keyclassname.Data()<<endl;
                in->cd(keyname.Data());

                TList* keylist2=gDirectory->GetListOfKeys();

                //-------------------looping in dir for categories--------------------------------------
                for(Int_t j=0;j<keylist2->LastIndex()+1;j++)
                {
                    TKey* key2  =(TKey*)keylist2->At(j);
                    keyname     = key2->GetName();
                    keyclassname= key2->GetClassName();

                    if(keyclassname.Contains("Category")!=0)
                    {
                        if(useClassList)
                        {
                            if(HTool::findString(&myclassNames[0],numberclasses,keyname))
                            {
                                counter++;
                                classarray->Add(key2);
                            }
                        }
                        else
                        {
                            counter++;
                            classarray->Add(key2);
                        }
                    }
                }
            }
        }
        classarray->Expand(classarray->GetEntries());
        cout<<"Number of Categries: "<<classarray->GetEntries() <<endl;

    }



    TString branch;
    TString histname;
    TString classname;
    TString varname;


    //--------------------looping over categories to get all data members----------------------------

    if(!useDescription)
    {
        cout<<"Use description"<<endl;
        FILE* histout=fopen("treeHistDescription.txt","w");
        FILE* anaout =fopen("treeHistResults.txt","w");

        cout<<"-----------------------------------------------------------"<<endl;
        for(Int_t j=0;j<classarray->LastIndex()+1;j++)
        {

            cout<<"###########################################################"<<endl;

            TKey* k=(TKey*)classarray->At(j);
            classname=k->GetName();
            cout<<classname.Data()<<endl;

            TClass* cl=gROOT->GetClass(k->GetName(),1);
            TList* list=HTool::getListOfAllDataMembers(cl);
            Int_t size =list->LastIndex();

            for(Int_t i=0;i<size+1;i++)
            {
                TDataMember* member=(TDataMember*)list->At(i);
                varname = member->GetName();
                if(varname.Contains("fgIsA")==0)
                {
                    branch=classname+".fData."+varname;
                    cout<<branch.Data()<<endl;

                    in->cd();
                    T->Draw(branch,"","",events,0);
                    TH1F *htemp = (TH1F*)gPad->GetPrimitive("htemp");
                    if(htemp)
                    {
                        branch=classname+"."+varname;
                        htemp->SetName(branch);
                        htemp->SetTitle(branch);
                        HTool::printHistInfo(htemp);
                        HTool::writeHistInfo(htemp,anaout);

                        fprintf(histout,"%s , %i , %f , %f \n",
                                htemp->GetName(),
                                htemp->GetNbinsX(),
                                htemp->GetXaxis()->GetXmin(),
                                htemp->GetXaxis()->GetXmax());

                        out->cd();
                        htemp->Write();
                        in->cd();
                    }
                }
            }
        }
        fclose(histout);
        fclose(anaout);

    }
    else
    {

        cout<<"Do not use description"<<endl;
        if(useClassList){
            cout<<"Use ClassList"<<endl;
            HTool::readHistsDescription(histdescriptionfile,classarray,myclassNames,numberclasses);
        } else {
            cout<<"Do not use ClassList"<<endl;
            HTool::readHistsDescription(histdescriptionfile,classarray,0,0);
        }
        classarray->Expand(classarray->GetEntries());
        cout<<"Number of Hists: "<<classarray->GetEntries() <<endl;

        FILE* anaout =fopen("treeHistResults.txt","w");

        for(Int_t j=0;j<classarray->LastIndex()+1;j++)
        {
            cout<<"###########################################################"<<endl;

            TH1* h=(TH1*)classarray->At(j);
            classname=h->GetName();
            classname.Resize(classname.First("."));
            varname  =h->GetName();

            Int_t st=varname.Last('.');
            for(Int_t ii=0;ii<st+1;ii++){
                varname[ii]=' ';
            }
            varname.ReplaceAll(" ","");

            branch=classname+".fData."+varname+">>"+h->GetName();
            cout<<branch.Data()<<endl;

            in->cd();
            T->Draw(branch,"","",events,0);
            if(h)
            {
                HTool::printHistInfo(h);
                HTool::writeHistInfo(h,anaout);

                out->cd();
                h->Write();
                in->cd();
            }
        }
        fclose(anaout);
    }

    delete result;
    classarray->Delete();
    delete classarray;
    out->Save();
    out->Close();

    return kTRUE;
}
Bool_t HTool::drawHistComp(TString nameinp1,TString nameinp2,TString name,TCanvas* comp,Int_t padn)
{
    // Gets histogram with name name from 2 input files and plots them into
    // into a canvas. If the canvas pointer is 0 a canvas will be created.
    // if padn=1 they will be overlayed in on pad, if pad=2 and comp=0, a
    // new canvas with n pads will be created and the 2 hists will be plotted single
    // if n=2 and comp not equal 0, the 2 pads of comp will used to draw.
    if(name.CompareTo("")==0)
    {
        cout<<"drawHistComp(): No Histogram name specified!"<<endl;
        return kFALSE;
    }
    if(nameinp1.CompareTo("")==0||nameinp2.CompareTo("")==0)
    {
        cout<<"drawHistComp(): File names not specified !"<<endl;
        return kFALSE;
    }

    TFile* inp1=new TFile(nameinp1.Data(),"READ");
    if(inp1==0)
    {
        cout<<"drawHistComp(): File "<<nameinp1.Data()<<" does not exist !"<<endl;
        return kFALSE;
    }

    TFile* inp2=new TFile(nameinp2.Data(),"READ");
    if(inp2==0)
    {
        cout<<"drawHistComp(): File "<<nameinp2.Data()<<" does not exist !"<<endl;
        return kFALSE;
    }

    TString longlist="";
    Bool_t getAll=kFALSE;
    if(name.Contains("DrawAll")!=0)
    {
        getAll=kTRUE;
        name.ReplaceAll("DrawAll","");
        Int_t nclasses=0;

        TString* myclasses=HTool::parseString(name,nclasses,",",kFALSE);

        TList* keylist=gDirectory->GetListOfKeys();
        Int_t sizekey =keylist->LastIndex();
        TString keyname;
        TString keyclassname;

        for(Int_t i=0;i<sizekey+1;i++)
        {
            TKey* key   =(TKey*)keylist->At(i);
            keyname     = key->GetName();
            keyclassname= key->GetClassName();

            if(keyclassname.Contains("TH1")!=0)
            {
                TString temp=keyname;
                temp.Resize(temp.First("."));
                if(HTool::findString(&myclasses[0],nclasses,temp)) longlist=longlist+keyname+",";
            }
        }

    }

    Int_t size=0;TString* mynames=0;

    if(!getAll)mynames= HTool::parseString(name,size,",",kFALSE);
    else       mynames= HTool::parseString(longlist,size,",",kFALSE);
    if(comp==0)
    {   // only if no canvas from outside is used
        comp=new TCanvas("compare","compare",1000,800);

        if(padn==2&&size==1)
        {   // if 2 pads are used and no multiple input
            comp ->Divide(2,1);
        }
        else if(padn==1&&size==1)
        {   // if 1 pads are used and no multiple input
        }
        else
        {   // if multiple input is used
            Int_t dim2=0;
            Int_t dim1=(Int_t)sqrt((Float_t)size);
            if(dim1*(dim1+1)<size)dim2=dim1+2;
            else                  dim2=dim1+1;
            comp ->Divide(dim1,dim2);
        }
        if(size==1)
        {  // if no multiple input

            TH1* h1=HTool::getHist(inp1,name);
            TH1* h2=HTool::getHist(inp2,name);

            if(!h1 || !h2) return kFALSE;

            h1->SetDirectory(0);
            h2->SetDirectory(0);

            h1->SetLineColor(2);
            h2->SetLineColor(4);
            if(padn==1)
            {
                if(h1->GetMaximum()>=h2->GetMaximum())
                {
                    h1->DrawCopy();
                    h2->DrawCopy("same");
                }
                else
                {
                    h2->DrawCopy();
                    h1->DrawCopy("same");
                }
            }
            else
            {
                comp->cd(1);
                h1->DrawCopy();
                comp->cd(2);
                h2->DrawCopy();
            }
            HTool::printCompHistInfo(h1,h2);
        }
        else if(padn==1&&size==1)
        {   // if 1 pads are used and no multiple input

        }
        else
        {
            // if multiple input is used
            for(Int_t i=0;i<size;i++)
            {
                comp->cd(i+1);

                TH1* h1=HTool::getHist(inp1,mynames[i]);
                TH1* h2=HTool::getHist(inp2,mynames[i]);

                if(!h1 || !h2) return kFALSE;

                h1->SetDirectory(0);
                h2->SetDirectory(0);

                h1->SetLineColor(2);
                h2->SetLineColor(4);

                if(h1->GetMaximum()>=h2->GetMaximum())
                {
                    h1->DrawCopy();
                    h2->DrawCopy("same");
                }
                else
                {
                    h2->DrawCopy();
                    h1->DrawCopy("same");
                }
                HTool::printCompHistInfo(h1,h2);
            }
        }
    }
    else
    {   // only if canvas from outside is used

        TH1* h1=HTool::getHist(inp1,name);
        TH1* h2=HTool::getHist(inp2,name);

        if(!h1 || !h2) return kFALSE;

        h1->SetDirectory(0);
        h2->SetDirectory(0);

        h1->SetLineColor(2);
        h2->SetLineColor(4);

	if(padn==1)
	{
            comp->cd();
            if(h1->GetMaximum()>=h2->GetMaximum())
            {
                h1->DrawCopy();
                h2->DrawCopy("same");
            }
            else
            {
                h2->DrawCopy();
                h1->DrawCopy("same");
            }
            HTool::printCompHistInfo(h1,h2);
        }
        else if(padn==2)
        {
            comp->cd(1);
            h1->DrawCopy();
            comp->cd(2);
            h2->DrawCopy();
            HTool::printCompHistInfo(h1,h2);
        }
        inp1->Close();
        inp2->Close();
    }
    return kTRUE;
}
Bool_t HTool::compHistFiles(TString nameinp1,TString nameinp2,TString name)
{
    // Gets histogram with name name from 2 input files and compares them by
    // mean,rms,range,maximum,maxposition, entries....

    if(nameinp1.CompareTo("")==0||nameinp2.CompareTo("")==0)
    {
        cout<<"compHistFiles(): File names not specified !"<<endl;
        return kFALSE;
    }

    TFile* inp1=new TFile(nameinp1.Data(),"READ");
    if(inp1==0)
    {
        cout<<"compHistFiles(): File "<<nameinp1.Data()<<" does not exist !"<<endl;
        return kFALSE;
    }

    TFile* inp2=new TFile(nameinp2.Data(),"READ");
    if(inp2==0)
    {
        cout<<"compHistFiles(): File "<<nameinp2.Data()<<" does not exist !"<<endl;
        return kFALSE;
    }
    TString* mynames=0;
    Int_t size=0;
    Bool_t useList=kFALSE;

    if(name.CompareTo("")==0)
    {
        TString longlist="";
        Int_t nclasses=0;

        TString* myclasses=HTool::parseString(name,nclasses,",",kFALSE);

        TList* keylist=gDirectory->GetListOfKeys();
        Int_t sizekey =keylist->LastIndex();
        TString keyname;
        TString keyclassname;

        for(Int_t i=0;i<sizekey+1;i++)
        {
            TKey* key   =(TKey*)keylist->At(i);
            keyname     = key->GetName();
            keyclassname= key->GetClassName();

            if(keyclassname.Contains("TH1")!=0)
            {
                TString temp=keyname;
                temp.Resize(temp.First("."));
                if(HTool::findString(&myclasses[0],nclasses,temp)) longlist=longlist+keyname+",";
            }
        }
        mynames= HTool::parseString(longlist,size,",",kFALSE);
        useList=kTRUE;
    }

    inp1->cd();
    TList* keylist=gDirectory->GetListOfKeys();
    Int_t sizekey =keylist->LastIndex();
    TString keyname;
    TString keyclassname;

    for(Int_t i=0;i<sizekey+1;i++)
    {
        TKey* key   =(TKey*)keylist->At(i);
        keyname     = key->GetName();
        keyclassname= key->GetClassName();

        if(keyclassname.Contains("TH1")!=0)
        {
            if(useList)
            {
                if(HTool::findString(&mynames[0],size,keyname))
                {
                    TH1* h1=HTool::getHist(inp1,keyname);
                    TH1* h2=HTool::getHist(inp2,keyname);
                    if(! h1 || !h2) continue;
                    HTool::printCompHistInfo(h1,h2,1);
                }
            }
            else
            {
                TH1* h1=HTool::getHist(inp1,keyname);
                TH1* h2=HTool::getHist(inp2,keyname);
                if(! h1 || !h2) continue;
                HTool::printCompHistInfo(h1,h2,1);
            }
        }
    }
    return kTRUE;
}
Bool_t HTool::printHistInfo(TH1* h1)
{
    // prints information over hist to screen .
    if(!h1) return kFALSE;
    cout<<"-----------------------------------------------------------"<<endl;
    cout<<h1->GetName()<<endl;
    cout<<"             Mean      :"<<h1->GetMean()<<endl;
    cout<<"             RMS       :"<<h1->GetRMS()<<endl;
    cout<<"             Entries   :"<<h1->GetEntries()<<endl;
    cout<<"             Integral  :"<<h1->Integral()<<endl;
    cout<<"             Maximum   :"<<h1->GetMaximum()<<endl;
    cout<<"             Maximum X :"<<h1->GetBinCenter(h1->GetMaximumBin())<<endl;
    cout<<"             UnderFlow :"<<h1->GetBinContent(0)<<endl;
    cout<<"             OverFlow  :"<<h1->GetBinContent(h1->GetNbinsX()+1)<<endl;
    cout<<"             nBins     :"<<h1->GetNbinsX()<<endl;
    cout<<"             xMin      :"<<h1->GetXaxis()->GetXmin()<<endl;
    cout<<"             xMax      :"<<h1->GetXaxis()->GetXmax()<<endl;
    cout<<"-----------------------------------------------------------"<<endl;
    return kTRUE;
}
Bool_t HTool::printCompHistInfo(TH1* h1,TH1* h2,Int_t detail)
{
    // prints information over hist to screen .
    if(!h1 || !h2) return kFALSE;
    cout<<"-----------------------------------------------------------"<<endl;
    cout<<h1->GetName()<<endl;
    if(detail==1)
    {
        TH1* h1tmp=(TH1*)h1->Clone();
        TH1* h2tmp=(TH1*)h2->Clone();

        Double_t prob=h1->KolmogorovTest(h2);
        cout<<" Kolmogorov Prob.            : "<<prob<<endl;

        h1tmp->Add(h2tmp,-1);
        cout<<" Integral after Substr.      : "<<h1tmp->Integral()<<endl;
        cout<<" Maximum after Substr.       : "<<h1tmp->GetMaximum()<<endl;
        cout<<" Maximum X after Substr.     : "<<h1tmp->GetBinCenter(h1tmp->GetMaximumBin())<<endl;

        cout<<" Minimum after Substr.       : "<<h1tmp->GetMinimum()<<endl;
        cout<<" Minimum X after Substr.     : "<<h1tmp->GetBinCenter(h1tmp->GetMinimumBin())<<endl;

        delete h1tmp;
        delete h2tmp;

    }
    cout<<" Mean      :"<< setw(15) <<h1->GetMean()                        <<" : "<<h2->GetMean()<<endl;
    cout<<" RMS       :"<< setw(15) <<h1->GetRMS()                         <<" : "<<h2->GetRMS()<<endl;
    cout<<" Entries   :"<< setw(15) <<h1->GetEntries()                     <<" : "<<h2->GetEntries()<<endl;
    cout<<" Integral  :"<< setw(15) <<h1->Integral()                       <<" : "<<h2->Integral()<<endl;
    cout<<" Maximum   :"<< setw(15) <<h1->GetMaximum()                     <<" : "<<h2->GetMaximum()<<endl;
    cout<<" Maximum X :"<< setw(15) <<h1->GetBinCenter(h1->GetMaximumBin())<<" : "<<h2->GetBinCenter(h2->GetMaximumBin())<<endl;
    cout<<" UnderFlow :"<< setw(15) <<h1->GetBinContent(0)                 <<" : "<<h2->GetBinContent(0)<<endl;
    cout<<" OverFlow  :"<< setw(15) <<h1->GetBinContent(h1->GetNbinsX()+1) <<" : "<<h2->GetBinContent(h2->GetNbinsX()+1)<<endl;
    cout<<" nBins     :"<< setw(15) <<h1->GetNbinsX()                      <<" : "<<h2->GetNbinsX()<<endl;
    cout<<" xMin      :"<< setw(15) <<h1->GetXaxis()->GetXmin()            <<" : "<<h2->GetXaxis()->GetXmin()<<endl;
    cout<<" xMax      :"<< setw(15) <<h1->GetXaxis()->GetXmax()            <<" : "<<h2->GetXaxis()->GetXmax()<<endl;
    cout<<"-----------------------------------------------------------"<<endl;
    return kTRUE;
}
Bool_t HTool::writeHistInfo(TH1* h1,FILE* anaout)
{
    // writes information over hist to ascii file .
    if(!h1 || !anaout) return kFALSE;

    fprintf(anaout,"-----------------------------------------------------------\n");
    fprintf(anaout,"%s\n",h1->GetName());
    fprintf(anaout,"             Mean      : %f\n",h1->GetMean());
    fprintf(anaout,"             RMS       : %f\n",h1->GetRMS());
    fprintf(anaout,"             Entries   : %f\n",h1->GetEntries());
    fprintf(anaout,"             Integral  : %f\n",h1->Integral());
    fprintf(anaout,"             Maximum   : %f\n",h1->GetMaximum());
    fprintf(anaout,"             Maximum X : %f\n",h1->GetBinCenter(h1->GetMaximumBin()));
    fprintf(anaout,"             UnderFlow : %f\n",h1->GetBinContent(0));
    fprintf(anaout,"             OverFlow  : %f\n",h1->GetBinContent(h1->GetNbinsX()+1));
    fprintf(anaout,"             nBins     : %i\n",h1->GetNbinsX());
    fprintf(anaout,"             xMin      : %f\n",h1->GetXaxis()->GetXmin());
    fprintf(anaout,"             xMax      : %f\n",h1->GetXaxis()->GetXmax());
    fprintf(anaout,"-----------------------------------------------------------\n");

    return kTRUE;
}
