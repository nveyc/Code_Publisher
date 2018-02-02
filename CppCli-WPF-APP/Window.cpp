///////////////////////////////////////////////////////////////////////////
// Window.cpp - C++\CLI implementation of WPF Application                //
//          - Demo for CSE 687 Project #4                                //
//                                                                       //
// Chao Yang                                                             //
//                                                                       //
// Reference:                                                            //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015             //
///////////////////////////////////////////////////////////////////////////
/*
*  To run as a Windows Application:
*  - Set Project Properties > Linker > System > Subsystem to Windows
*  - Comment out int main(...) at bottom
*  - Uncomment int _stdcall WinMain() at bottom
*  To run as a Console Application:
*  - Set Project Properties > Linker > System > Subsytem to Console
*  - Uncomment int main(...) at bottom
*  - Comment out int _stdcall WinMain() at bottom
*/
#include "Window.h"
using namespace CppCliWindows;

WPFCppCliDemo::WPFCppCliDemo()
{
  // set up channel
  ObjectFactory* pObjFact = new ObjectFactory;
  pSendr_ = pObjFact->createSendr();
  pRecvr_ = pObjFact->createRecvr();
  pClient_ = pObjFact->createClient();
  pChann_ = pObjFact->createMockChannel(pSendr_, pRecvr_,pClient_);
  pClient_->setClient();
  pChann_->start();
  pClient_->setSender(pSendr_);
  delete pObjFact;
  // client's receive thread
  recvThread = gcnew Thread(gcnew ThreadStart(this, &WPFCppCliDemo::getMessage));
  recvThread->Start();
  // set event handlers
  this->Loaded += 
    gcnew System::Windows::RoutedEventHandler(this, &WPFCppCliDemo::OnLoaded);
  this->Closing += 
    gcnew CancelEventHandler(this, &WPFCppCliDemo::Unloading);
  hFolderBrowseButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::browseForFolder);
  hShowItemsButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::getItemsFromList);
  hShowCatagoryButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::ShowAllCatagory);
  hShowFileinCatagoryButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::ShowFilesInCatagory);
  hUploadItemButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::UploadFile);
  hDownloadButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::DownloadFile);
  hShowNoParentFile->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::ShowFilesWithNP);
  hDisplayhtmlButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::ShowHTML);
  hAnalysisButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::AnalyzeRepo);
  hDeleteButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::DeleteFile);
  hDemoButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::Demo);
  // set Window properties
  this->Title = "WPF C++/CLI Demo";
  this->Width = 900;
  this->Height = 650;
  // attach dock panel to Window
  this->Content = hDockPanel;
  hDockPanel->Children->Add(hStatusBar);
  hDockPanel->SetDock(hStatusBar, Dock::Bottom);
  hDockPanel->Children->Add(hGrid);
  // setup Window controls and views
  setUpTabControl();
  setUpStatusBar();
  setUpSendMessageView();
  setUpLoadView();
  setUpConnectionView();
  setUpRepoView();
}

WPFCppCliDemo::~WPFCppCliDemo()
{
  delete pChann_;
  delete pSendr_;
  delete pRecvr_;
}

void WPFCppCliDemo::setUpStatusBar()
{
  hStatusBar->Items->Add(hStatusBarItem);
  hStatus->Text = "very important messages will appear here";
  hStatusBarItem->Content = hStatus;
  hStatusBar->Padding = Thickness(10, 2, 10, 2);
}

void WPFCppCliDemo::setUpTabControl()
{
  hGrid->Children->Add(hTabControl);
  hSendMessageTab->Header = "Demo";
  hUploadFileTab->Header = "Upload Files";
  hRepoTab->Header = "Repository";
  hTabControl->Items->Add(hSendMessageTab);
  hTabControl->Items->Add(hUploadFileTab);
  hTabControl->Items->Add(hRepoTab);
}

void WPFCppCliDemo::setTextBlockProperties()
{
  RowDefinition^ hRow1Def = gcnew RowDefinition();
  hSendMessageGrid->RowDefinitions->Add(hRow1Def);
  Border^ hBorder1 = gcnew Border();
  hBorder1->BorderThickness = Thickness(1);
  hBorder1->BorderBrush = Brushes::Black;
}

void WPFCppCliDemo::setButtonsProperties()
{
  RowDefinition^ hRow2Def = gcnew RowDefinition();
  hRow2Def->Height = GridLength(75);
  hSendMessageGrid->RowDefinitions->Add(hRow2Def);
  hDemoButton->Content = "Demo";
  Border^ hBorder2 = gcnew Border();
  hBorder2->Width = 120;
  hBorder2->Height = 30;
  hBorder2->BorderThickness = Thickness(1);
  hBorder2->BorderBrush = Brushes::Black;
  hBorder2->Child = hDemoButton;
  Border^ hBorder3 = gcnew Border();
  hBorder3->Width = 120;
  hBorder3->Height = 30;
  hBorder3->BorderThickness = Thickness(1);
  hBorder3->BorderBrush = Brushes::Black;
  hStackPanel1->Children->Add(hBorder2);
  TextBlock^ hSpacer = gcnew TextBlock();
  hSpacer->Width = 10;
  hStackPanel1->Children->Add(hSpacer);
  hStackPanel1->Children->Add(hBorder3);
  hStackPanel1->Orientation = Orientation::Horizontal;
  hStackPanel1->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
  hSendMessageGrid->SetRow(hStackPanel1, 1);
  hSendMessageGrid->Children->Add(hStackPanel1);
}

void WPFCppCliDemo::setUpSendMessageView()
{
  Console::Write("\n  setting up sendMessage view");
  hSendMessageGrid->Margin = Thickness(20);
  hSendMessageTab->Content = hSendMessageGrid;
  
  hDemoButton->Content = "Demo";
  hDemoButton->Height = 30;
  hDemoButton->Width = 200;
  hDemoButton->BorderThickness = Thickness(1);
  hDemoButton->BorderBrush = Brushes::Black;
  hSendMessageGrid->SetRow(hDemoButton, 0);
  hSendMessageGrid->SetColumn(hDemoButton, 0);
  hSendMessageGrid->Children->Add(hDemoButton);
}

std::string WPFCppCliDemo::toStdString(String^ pStr)
{
  std::string dst;
  for (int i = 0; i < pStr->Length; ++i)
    dst += (char)pStr[i];
  return dst;
}


String^ WPFCppCliDemo::toSystemString(std::string& str)
{
  StringBuilder^ pStr = gcnew StringBuilder();
  for (size_t i = 0; i < str.size(); ++i)
    pStr->Append((Char)str[i]);
  return pStr->ToString();
}

void WPFCppCliDemo::addText(String^ msg)
{
  for each(String^ item in msg->Split(':'))
  {
	  if(item != "")
		  hRepoBox->Items->Add(item);
  }
}

void WPFCppCliDemo::getMessage()
{
  // recvThread runs this function

  while (true)
  {
    std::cout << "\n  receive thread calling getMessage()";
    std::string msg = pRecvr_->getMessage();
    String^ sMsg = toSystemString(msg);
    array<String^>^ args = gcnew array<String^>(1);
    args[0] = sMsg;

    Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addText);
    Dispatcher->Invoke(act, args);  // must call addText on main UI thread
  }
}


void WPFCppCliDemo::getItemsFromList(Object^ sender, RoutedEventArgs^ args)
{
	int index = 0;
	int count = hListBox->SelectedItems->Count;
	hStatus->Text = "Show Selected Items";
	array<System::String^>^ items = gcnew array<String^>(count);
	if (count > 0) {
		for each (String^ item in hListBox->SelectedItems)
		{
			items[index++] = item;
		}
	}
	hListBox->Items->Clear();
	if (count > 0) {
		for each (String^ item in items)
		{
			hListBox->Items->Add(item);
		}
	}
}

void WPFCppCliDemo::setUploadRomAndCol()
{
	hFileListGrid->Margin = Thickness(20);
	hUploadFileTab->Content = hFileListGrid;

	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hRow1Def->Height = GridLength(45);
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(380);
	RowDefinition^ hRow3Def = gcnew RowDefinition();
	hRow3Def->Height = GridLength(45);
	RowDefinition^ hRow4Def = gcnew RowDefinition();
	hRow1Def->Height = GridLength(45);
	hFileListGrid->RowDefinitions->Add(hRow1Def);
	hFileListGrid->RowDefinitions->Add(hRow2Def);
	hFileListGrid->RowDefinitions->Add(hRow3Def);
	hFileListGrid->RowDefinitions->Add(hRow4Def);

	ColumnDefinition^ hCol1Def = gcnew ColumnDefinition();
	ColumnDefinition^ hCol2Def = gcnew ColumnDefinition();
	ColumnDefinition^ hCol3Def = gcnew ColumnDefinition();
	ColumnDefinition^ hCol4Def = gcnew ColumnDefinition();

	hFileListGrid->ColumnDefinitions->Add(hCol1Def);
	hFileListGrid->ColumnDefinitions->Add(hCol2Def);
	hFileListGrid->ColumnDefinitions->Add(hCol3Def);
	hFileListGrid->ColumnDefinitions->Add(hCol4Def);
}
void WPFCppCliDemo::setUpLoadButtoms()
{

	hFolderBrowseButton->Content = "Browse Directory";
	hFolderBrowseButton->Height = 30;
	hFolderBrowseButton->Width = 200;
	hFolderBrowseButton->BorderThickness = Thickness(1);
	hFolderBrowseButton->BorderBrush = Brushes::Black;
	hFileListGrid->SetRow(hFolderBrowseButton, 3);
	hFileListGrid->SetColumn(hFolderBrowseButton, 0);
	hFileListGrid->Children->Add(hFolderBrowseButton);

	// Show selected items button.
	hShowItemsButton->Content = "Show Selected Items";
	hShowItemsButton->Height = 30;
	hShowItemsButton->Width = 200;
	hShowItemsButton->BorderThickness = Thickness(1);
	hShowItemsButton->BorderBrush = Brushes::Black;
	hFileListGrid->SetRow(hShowItemsButton, 3);
	hFileListGrid->SetColumn(hShowItemsButton, 1);
	hFileListGrid->Children->Add(hShowItemsButton);

	// Upload selected items button.
	hUploadItemButton->Content = "Upload Selected Items";
	hUploadItemButton->Height = 30;
	hUploadItemButton->Width = 200;
	hUploadItemButton->BorderThickness = Thickness(1);
	hUploadItemButton->BorderBrush = Brushes::Black;
	hFileListGrid->SetRow(hUploadItemButton, 3);
	hFileListGrid->SetColumn(hUploadItemButton, 3);
	hFileListGrid->Children->Add(hUploadItemButton);
}

void WPFCppCliDemo::setUpLoadView()
{
  setUploadRomAndCol();
  setUpLoadButtoms();

  Border^ hBorder1 = gcnew Border();
  hBorder1->BorderThickness = Thickness(1);
  hBorder1->BorderBrush = Brushes::Black;
  hListBox->SelectionMode = SelectionMode::Multiple;
  hBorder1->Child = hListBox;
  hFileListGrid->SetRow(hBorder1, 1);
  hFileListGrid->SetColumn(hBorder1, 0);
  hFileListGrid->SetColumnSpan(hBorder1, 4);
  //hFileListGrid->SetRowSpan(hBorder1, 2);
  hFileListGrid->Children->Add(hBorder1);
  // file label
  fileLable->Content = "File List";
  fileLable->Height = 30;
  fileLable->Width = 200;
  fileLable->BorderThickness = Thickness(0);
  hFileListGrid->SetRow(fileLable, 0);
  hFileListGrid->SetColumn(fileLable, 0);
  hFileListGrid->Children->Add(fileLable);

  // category label
  categoryLable->Content = "     Input Category Name Here";
  categoryLable->Height = 30;
  categoryLable->Width = 200;
  categoryLable->BorderThickness = Thickness(0);
  hFileListGrid->SetRow(categoryLable, 2);
  hFileListGrid->SetColumn(categoryLable, 2);
  hFileListGrid->Children->Add(categoryLable);
  //category
  Category->Height = 30;
  Category->Width = 200;
  hFileListGrid->SetRow(Category, 3);
  hFileListGrid->SetColumn(Category, 2);
  hFileListGrid->Children->Add(Category);
  hFolderBrowserDialog->ShowNewFolderButton = false;
  hFolderBrowserDialog->SelectedPath = System::IO::Directory::GetCurrentDirectory();
}


void WPFCppCliDemo::setUpRepoRomAndCol()
{
	Console::Write("\n  setting up RepoList view");
	hRepoGrid->Margin = Thickness(20);
	hRepoTab->Content = hRepoGrid;

	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hRow1Def->Height = GridLength(90);
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(90);
	RowDefinition^ hRow3Def = gcnew RowDefinition();
	hRow3Def->Height = GridLength(90);
	RowDefinition^ hRow4Def = gcnew RowDefinition();
	hRow4Def->Height = GridLength(90);
	RowDefinition^ hRow5Def = gcnew RowDefinition();
	hRow5Def->Height = GridLength(90);
	RowDefinition^ hRow6Def = gcnew RowDefinition();
	hRow6Def->Height = GridLength(90);
	hRepoGrid->RowDefinitions->Add(hRow1Def);
	hRepoGrid->RowDefinitions->Add(hRow2Def);
	hRepoGrid->RowDefinitions->Add(hRow3Def);
	hRepoGrid->RowDefinitions->Add(hRow4Def);
	hRepoGrid->RowDefinitions->Add(hRow5Def);
	hRepoGrid->RowDefinitions->Add(hRow6Def);


	ColumnDefinition^ hCol1Def = gcnew ColumnDefinition();
	hCol1Def->Width = GridLength(220);
	ColumnDefinition^ hCol2Def = gcnew ColumnDefinition();
	hCol2Def->Width = GridLength(300);
	ColumnDefinition^ hCol3Def = gcnew ColumnDefinition();
	hCol3Def->Width = GridLength(300);

	hRepoGrid->ColumnDefinitions->Add(hCol1Def);
	hRepoGrid->ColumnDefinitions->Add(hCol2Def);
	hRepoGrid->ColumnDefinitions->Add(hCol3Def);
}


void WPFCppCliDemo::sethAnalysisButton()
{
	hAnalysisButton->Content = "Code Analyze";
	hAnalysisButton->Height = 30;
	hAnalysisButton->Width = 150;
	hAnalysisButton->BorderThickness = Thickness(1);
	hAnalysisButton->BorderBrush = Brushes::Black;
	hRepoGrid->SetRow(hAnalysisButton, 0);
	hRepoGrid->SetColumn(hAnalysisButton, 0);
	hRepoGrid->Children->Add(hAnalysisButton);
}
void WPFCppCliDemo::sethShowCatagoryButton()
{
	hShowCatagoryButton->Content = "All Categories";
	hShowCatagoryButton->Height = 30;
	hShowCatagoryButton->Width = 150;
	hShowCatagoryButton->BorderThickness = Thickness(1);
	hShowCatagoryButton->BorderBrush = Brushes::Black;
	hRepoGrid->SetRow(hShowCatagoryButton, 1);
	hRepoGrid->SetColumn(hShowCatagoryButton, 0);
	hRepoGrid->Children->Add(hShowCatagoryButton);
}
void WPFCppCliDemo::sethShowFileinCatagoryButton()
{

	hShowFileinCatagoryButton->Content = "Files in Category";
	hShowFileinCatagoryButton->Height = 30;
	hShowFileinCatagoryButton->Width = 150;
	hShowFileinCatagoryButton->BorderThickness = Thickness(1);
	hShowFileinCatagoryButton->BorderBrush = Brushes::Black;
	hRepoGrid->SetRow(hShowFileinCatagoryButton, 2);
	hRepoGrid->SetColumn(hShowFileinCatagoryButton, 0);
	hRepoGrid->Children->Add(hShowFileinCatagoryButton);
}
void WPFCppCliDemo::sethDisplayhtmlButton()
{
	hDisplayhtmlButton->Content = "HTML File";
	hDisplayhtmlButton->Height = 30;
	hDisplayhtmlButton->Width = 150;
	hDisplayhtmlButton->BorderThickness = Thickness(1);
	hDisplayhtmlButton->BorderBrush = Brushes::Black;
	hRepoGrid->SetRow(hDisplayhtmlButton, 3);
	hRepoGrid->SetColumn(hDisplayhtmlButton, 0);
	hRepoGrid->Children->Add(hDisplayhtmlButton);
}
void WPFCppCliDemo::sethDownloadButton()
{
	hDownloadButton->Content = "Download";
	hDownloadButton->Height = 30;
	hDownloadButton->Width = 150;
	hDownloadButton->BorderThickness = Thickness(1);
	hDownloadButton->BorderBrush = Brushes::Black;
	hRepoGrid->SetRow(hDownloadButton, 5);
	hRepoGrid->SetColumn(hDownloadButton, 1);
	hRepoGrid->Children->Add(hDownloadButton);
}
void WPFCppCliDemo::sethShowNoParentFile()
{
	hShowNoParentFile->Content = "No Parent File";
	hShowNoParentFile->Height = 30;
	hShowNoParentFile->Width = 150;
	hShowNoParentFile->BorderThickness = Thickness(1);
	hShowNoParentFile->BorderBrush = Brushes::Black;
	hRepoGrid->SetRow(hShowNoParentFile, 4);
	hRepoGrid->SetColumn(hShowNoParentFile, 0);
	hRepoGrid->Children->Add(hShowNoParentFile);
}
void WPFCppCliDemo::sethDeleteButton()
{
	hDeleteButton->Content = "Delete";
	hDeleteButton->Height = 30;
	hDeleteButton->Width = 150;
	hDeleteButton->BorderThickness = Thickness(1);
	hDeleteButton->BorderBrush = Brushes::Black;
	hRepoGrid->SetRow(hDeleteButton, 5);
	hRepoGrid->SetColumn(hDeleteButton, 2);
	hRepoGrid->Children->Add(hDeleteButton);
}


void WPFCppCliDemo::setUpRepoView()
{
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hRepoBox->SelectionMode = SelectionMode::Multiple;
	hBorder1->Child = hRepoBox;
	hRepoGrid->SetRow(hBorder1, 0);
	hRepoGrid->SetColumn(hBorder1, 1);
	hRepoGrid->SetColumnSpan(hBorder1, 2);
	hRepoGrid->SetRowSpan(hBorder1, 5);
	hRepoGrid->Children->Add(hBorder1);

	setUpRepoRomAndCol();
	sethAnalysisButton();
	sethShowCatagoryButton();
	sethShowFileinCatagoryButton();
	sethDisplayhtmlButton();
	sethDownloadButton();
	sethShowNoParentFile();
	sethDeleteButton();
}

void WPFCppCliDemo::browseForFolder(Object^ sender, RoutedEventArgs^ args)
{
  std::cout << "\n  Browsing for folder";
  hListBox->Items->Clear();
  System::Windows::Forms::DialogResult result;
  result = hFolderBrowserDialog->ShowDialog();
  if (result == System::Windows::Forms::DialogResult::OK)
  {
    String^ path = hFolderBrowserDialog->SelectedPath;
    std::cout << "\n  opening folder \"" << toStdString(path) << "\"";
    array<String^>^ files = System::IO::Directory::GetFiles(path, L"*.*");
    for (int i = 0; i < files->Length; ++i)
      hListBox->Items->Add(files[i]);
    array<String^>^ dirs = System::IO::Directory::GetDirectories(path);
    for (int i = 0; i < dirs->Length; ++i)
      hListBox->Items->Add(L"<> " + dirs[i]);
  }
}
void WPFCppCliDemo::setUpConnectionView()
{
  Console::Write("\n  setting up Connection view");
}

void WPFCppCliDemo::OnLoaded(Object^ sender, RoutedEventArgs^ args)
{
  Console::Write("\n  Window loaded");
}
void WPFCppCliDemo::Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args)
{
  Console::Write("\n  Window closing");
}
void WPFCppCliDemo::ShowAllCatagory(Object^ sender, RoutedEventArgs^ args)
{
	pClient_->reAllCategory();
	hRepoBox->Items->Clear();
	mode = 1;
}
void WPFCppCliDemo::ShowFilesInCatagory(Object^ sender, RoutedEventArgs^ args)
{
	if (hRepoBox->SelectedItem == nullptr)
		return;
	selectCategory = hRepoBox->SelectedItem->ToString();
	pClient_->reFileInCategory(toStdString(hRepoBox->SelectedItem->ToString()));
	hRepoBox->Items->Clear();
	mode = 2;
}

void WPFCppCliDemo::DownloadFile(Object^ sender, RoutedEventArgs^ args) 
{
	if (hRepoBox->SelectedItem == nullptr)
		return;
	for each(String^ item in hRepoBox->SelectedItems)
	{
		pClient_->downLoad(toStdString(item),toStdString(selectCategory));
	}
}
void WPFCppCliDemo::ShowFilesWithNP(Object^ sender, RoutedEventArgs^ args) {
	std::string select = "";
	if (hRepoBox->SelectedItem == nullptr)
		return;
	selectCategory = hRepoBox->SelectedItem->ToString();
	select = toStdString(hRepoBox->SelectedItem->ToString());
	hRepoBox->Items->Clear();
	pClient_->reFileWithNP(select);	
	mode = 2;
}
void WPFCppCliDemo::ShowHTML(Object^ sender, RoutedEventArgs^ args)
{	
	selectCategory = "HTML";
	pClient_->reHTML();
	hRepoBox->Items->Clear();
	mode = 2;
}

void WPFCppCliDemo::UploadFile(Object^ sender, RoutedEventArgs^ args)
{
	std::string cate = toStdString(Category->Text->ToString());
	for each(String^ item in hListBox->SelectedItems)
	{
		pClient_->allCategories(toStdString(item), cate);
	}
}

void WPFCppCliDemo::AnalyzeRepo(Object^ sender, RoutedEventArgs^ args) {
	
	pClient_->reCodeAnalysis();
	hRepoBox->Items->Clear();
}


void WPFCppCliDemo::Uploadfiles(Object^ sender, RoutedEventArgs^ args)
{
	if (hRepoBox->SelectedItem == nullptr)
		return;
	std::string category = toStdString(Category->Text);
	for each(String^ item in hRepoBox->SelectedItems)
	{
		pClient_->allCategories(toStdString(item),category);
	}
}

void WPFCppCliDemo::DeleteFile(Object^ sender, RoutedEventArgs^ args)
{
	if (hRepoBox->SelectedItem == nullptr)
		return;

	if (mode == 1)
	{
		for each(String^ item in hRepoBox->SelectedItems)
		{
			pClient_->reDelete("none", toStdString(item->ToString()));
		}
		hRepoBox->Items->Clear();
		pClient_->reAllCategory();
	}
	else
	{
		for each(String^ item in hRepoBox->SelectedItems)
		{
			if (toStdString(item->ToString()) == "script.js" || toStdString(item->ToString()) == "stylesheet.css")
				return;
			pClient_->reDelete(toStdString(item->ToString()), toStdString(selectCategory));
		}
		hRepoBox->Items->Clear();
		if (selectCategory == "HTML")
			pClient_->reHTML();
		else
			pClient_->reFileInCategory(toStdString(selectCategory));
	}
}

void WPFCppCliDemo::Demo(Object^ sender, RoutedEventArgs^ args)
{
	pClient_->demo();
}


[STAThread]
//int _stdcall WinMain()
int main(array<System::String^>^ args)
{
  Console::WriteLine(L"\n Starting WPFCppCliDemo");

  Application^ app = gcnew Application();
  app->Run(gcnew WPFCppCliDemo());
  Console::WriteLine(L"\n\n");
}