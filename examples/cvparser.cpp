/* A CV parser created from Emorfili Terzimpasoglou using the c++ 
 * podofo library. Tested with file 'CV.pdf'.  
*/
#include <iostream>
#include <cstdio>
#include <vector>
#include <stack>
#include <sstream>
#include <podofo/podofo.h>

using namespace PoDoFo;
using namespace std;

//DEFINE SOME GLOBAL VECTORS 
vector<string> letters; //vector for characters of the pdf
vector<string> allwords; //vector for words of the pdf

//DISPLAY HELP MESSAGE IN CASE OF WRONG USAGE OF THE MAIN FUNCTION
void PrintUsage() 
{
    cout << "Usage:" << endl;
    cout << "./cvparser [inputcv.pdf]" << endl;

}

/* The TextProcessor function a found text string in the PDF and converts it first to unicode
 * and then to a readable c string, which is saved in a vector of strings for later use. This 
 * function is called from within the ReadCV() function, which locates the PDF text data. 
 */
void TextProcessor(PdfFont* txtfont, const PdfString & rString)
{   
    PdfString unicode = txtfont->GetEncoding()->ConvertToUnicode(rString,txtfont);
    letters.push_back(unicode.GetStringUtf8().c_str());
}

/* ReadCV is the main PDF parsing function. Here the tokens (operators and their parameters) that 
 * generate the PDF glyphs and text are processed in order to flesh out the text data of the PDF.
 * This function is called from within a loop in the function PageIterator(), which iterates over all
 * PDF pages. It takes the PDF document and its page as input variables.
 */
void ReadCV(PdfMemDocument* document, PdfPage* page)
{
    const char* token = nullptr; //token variable
    PdfVariant var; //variable to save the operator parameters
    EPdfContentsType type; //type of the token (operator/keyword or parameter/variable)

    PdfContentsTokenizer tokenizer(page);

    PdfFont* txtfont = nullptr;
    bool istext = false;
       
    stack<PdfVariant> varstack; //define a stack to fill parameters in

    while(tokenizer.ReadNext(type,token,var))
    {
        switch(type)
        {
            case ePdfContentsType_Variant: //if token is a parameter, push it into the stack
                varstack.push(var);
                break;
            case ePdfContentsType_Keyword: //if token is an operator, check its name and parameters
                if(strcmp(token,"l") == 0 || strcmp(token,"m") == 0) //The text matrix operator, 
                                                                     //specifies a starting position for the text
                {
                        while( !varstack.empty() )
                        varstack.pop();
                }
                else if(strcmp(token,"BT") == 0) //begins a text object
                {
                    istext = true;     
                }                
                else if(strcmp(token,"ET") == 0) //ends a text object
                {
                    if( !istext ) 
                        printf("WARNING: Found ET without BT!\n");
                }
                //here process operators within a text object                
                if(istext)
                {
                    if(strcmp(token,"Tf") == 0) //text font operator 
                    {
                        if(varstack.size() < 2)
                        {
                            printf("WARNING: Expects two arguments for 'Tf', ignoring\n" );
                            txtfont = nullptr;
                            continue;
                        }
                        varstack.pop();
                        PdfName fontName = varstack.top().GetName();
                        PdfObject* pFont = page->GetFromResources(PdfName("Font"),fontName);
                        if(!pFont) 
                        {
                            PODOFO_RAISE_ERROR_INFO(ePdfError_InvalidHandle, "Cannot create font!");
                        }
                        txtfont = document->GetFont(pFont);
                    }
                    //all the operators below are text writing operators and thus call on the TextProcessor 
                    //function to process the PDF text strings
                    else if(strcmp(token,"Tj")== 0 || strcmp(token,"'") == 0)
                    {
                        if(varstack.size() < 1)
                        {
                            printf("WARNING: Expects one argument for '%s', ignoring\n", token);
                            continue;
                        }
                        TextProcessor(txtfont,varstack.top().GetString());
                        varstack.pop();
                    }
                    else if( strcmp(token, "\"" ) == 0)
                    {
                        if(varstack.size() < 3)
                        {
                            printf("WARNING: Expects three arguments for '%s', ignoring\n", token);
                            while(!varstack.empty())
                            varstack.pop();
                            continue;
                        }
                        cout << "[      \":    ]" << endl;
                        TextProcessor(txtfont,varstack.top().GetString());
                        varstack.pop();
                        varstack.pop(); // remove char spacing from varstack
                        varstack.pop(); // remove word spacing from varstack
                    }
                    else if( strcmp(token,"TJ") == 0) 
                    {
                        if(varstack.size() < 3)
                        {
                            printf("WARNING: Expects one argument for '%s', ignoring\n", token);
                            continue;
                        }
                        PdfArray array = varstack.top().GetArray();
                        varstack.pop();
                    
                        for( int i=0; i<static_cast<int>(array.GetSize()); i++ ) 
                        {
                            if( array[i].IsString() || array[i].IsHexString() )
                            {
                              TextProcessor(txtfont,array[i].GetString());
                            }
                        }
                    }
                }
                break;
            default: //if the token is neither an operator nor a parameter, raise an error
                PODOFO_RAISE_ERROR(ePdfError_InternalLogic);
                break;
        }
    }
}

/* PrintCV() processes the text withdrawn from the CV PDF and writes them on the terminal in 
 * a structured manner. 
 */
void PrintCV()
{
    //CREATE WORDS OUT OF CHARACTERS and save them in a vector of words
    ostringstream wordstream("",ios_base::ate);
    for(int i=0;i<letters.size();i++)
    {
        if(letters.at(i)!=" ")
        {
            wordstream << letters.at(i);
        }
        else if(letters.at(i)==" ")
        {
            string word;
            word = wordstream.str();
            if(word!="")
            {
                allwords.push_back(word);
            }

        wordstream.str("");
        word ="";

        }
    }
    
    //WRITE ON TERMINAL
    //define some keywords
    vector<string> keywords = {"ACHIEVEMENTS","PROFESSIONAL","EXPERIENCE","EDUCATION","Frontend"};
    
    for(int i=0;i<allwords.size();i++) //iterate over all words
    {
        const char* term = allwords.at(i).c_str();
        //structure titles
        if(strstr(term,"EDUCATION")!=0 || strstr(term,"EXPERIENCE")!=0 || strstr(term,"ACHIEVEMENTS")!=0)
        {
            cout << term << endl;
        }
        //structure bullet lines
        else if(strstr(term,"•")!=0 || strstr(term,"●")!=0)
        {
            cout << endl;
            cout << term; //print bullet
            i++; //move to word after bullet
            if(i<allwords.size()) //make sure the word after the bullet is within the 'allwords' vector size.
            {
                term = allwords.at(i).c_str();
            }                
            else
            {
                i--;
                continue;
            }
            
            bool iskeyword = false; //used to determine where bullet line ends
            //write bullet line
            while(strstr(term,"•")==0 && strstr(term,"●")==0)
            {
                //check whether word is a keyword,i.e. not part of the bullet line
                //if it is a keyword, break while.
                for(int j=0;j<keywords.size();j++)
                {
                    if(strstr(term,keywords.at(j).c_str()))
                    {
                        cout << endl;
                        iskeyword = true;
                    }
                }
                if(iskeyword)
                {
                    break;
                }
                //print word in bullet line and move to next word, check that next word is within the allwords vector size
                cout << " " << term << " ";
                i++;
                if(i>=allwords.size())
                {
                    i--;
                    break;
                }
                else
                {
                    term = allwords.at(i).c_str();
                }          
            }
            //while stops at point where it encounters another bullet or a keyword, 
            //in order to process it in the wider for loop, 'i' must be reduced first 
            //and a new line must be entered for structure purposes
            cout << endl;
            i--;
        }
        else //write everything else
        {
            cout << term << " ";
        }
    }
}

/* The PageIterator() open the PDF document and loops over all the pages of the PDF. 
 * It takes the PDF input filename as a variable. For each page, it calls on the ReadCV() 
 * function to read out the content of each page. After it has read all the pages,
 *  it calls on the PrintCV() function to print them on the terminal.
 */
void PageIterator(const char* InputFilename)
{
    PdfMemDocument pdf;
    pdf.Load(InputFilename);
    int pCount = pdf.GetPageCount();

    for(int i=0;i<pCount;i++){
        PdfPage* page = pdf.GetPage(i);
        ReadCV(&pdf,page);
    }
    PrintCV();
}

//MAIN FUNCTION
int main(int argc, char* argv[]){

    //check main function usage
    if( argc != 2 )
    {
        PrintUsage();
        return -1;
    }

    //call the PageIterator() function and catch for errors
    try {
        PageIterator(argv[1]);
    } catch( PdfError & eCode ) {
        eCode.PrintErrorMsg();
        return eCode.GetError();
    }

    //check for memory errors
    try {
        PdfEncodingFactory::FreeGlobalEncodingInstances();
    } catch( PdfError & eCode ) {
        eCode.PrintErrorMsg();
        return eCode.GetError();
    }
}