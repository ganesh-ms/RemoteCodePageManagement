/////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs - GUI for OOD Project #3                     //
//                      Code Publisher Client GUI                  //
// ver			     : 1.2                                             //
// Author		     : Ganesh Mamatha Sheshappa                        //
// Language      : Visual C++, Visual Studio 2017                  //
// Platform	     : Mac running Windows 10                          //
// Application   : Prototype for CSE687 - OOD Project-3            //
// Source        : Jim Fawcett, CSE687 - Object Oriented Design,   //
//                 Fall 2018  - GUI for Project3HelpWPF            //
/////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package defines one class MainWindow that provides tabs:
 * Navigate Server Files: Navigate through server directory to find files for testing
 *      - Shows server directories and files
 *      - Can navigate by double clicking directories
 * Converted File:  Can see and select the published (Converted) files 
 *      - Shows the files which are published and received from the server using 
 *        the Native C++ code
 *      - Double clicking will open the published files in the default browser or 
 *        the popup window based on the radio button selected
 *   
 * Required Files:
 * ---------------
 * Mainwindow.xaml, MainWindow.xaml.cs
 * SelectionWindow.xaml, SelectionWindow.xaml.cs
 * Translater.dll
 * 
 * Maintenance History:
 * --------------------
 * ver 1.2 : 04 Apr 2019
 * - added the publish button, pattern & regex text box, subdirectory checkbox
 *   added a tab to see and open the Published/converted files
 * ver 1.1 : 07 Aug 2018
 * - fixed bug in DirList_MouseDoubleClick by returning when selectedDir is null
 * ver 1.0 : 30 Mar 2018
 * - first release
 * - Several early prototypes were discussed in class. Those are all superceded
 *   by this package.
 */

// Translater has to be statically linked with CommLibWrapper
// - loader can't find Translater.dll dependent CommLibWrapper.dll
using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Threading;
using MsgPassingCommunication;

namespace WpfApp1
{
  public partial class MainWindow : Window
  {
    
    string workingDir = "../../../../ClientDirectory"; //Receiving Directory for the Client  
    string clientAddress { get; set; } //Stores the Client Address
    int clientPort { get; set; }//Stores the Client Port
    string serverAddress { get; set; }//Stores the Server Address
    int serverPort { get; set; }//Stores the Server Port
    private string Patterns { get; set; }//Stores the patterns information
    private bool openOnce = false;//Bool variable to make sure the files are opened once in the browser or popup
    private string Regex_ { get; set; }//Stores the regex information
    public SelectionWindow swin { get; set; } = null;//Selection Window object

    public MainWindow()
    {
      InitializeComponent();//Initializes all the components
    }

    private Stack<string> pathStack_ = new Stack<string>();
    private Translater translater;
    private CsEndPoint endPoint_;
    private Thread rcvThrd = null;
    private Dictionary<string, Action<CsMessage>> dispatcher_ 
      = new Dictionary<string, Action<CsMessage>>();

    //----< process incoming messages on child thread >----------------
    private void processMessages()
    {
      ThreadStart thrdProc = () => {
        while (true)
        {
          CsMessage msg = translater.getMessage();
          string msgId = msg.value("command");
          if (dispatcher_.ContainsKey(msgId))
            dispatcher_[msgId].Invoke(msg);
        }
      };
      rcvThrd = new Thread(thrdProc);
      rcvThrd.IsBackground = true;
      rcvThrd.Start();
    }

    //----< function dispatched by child thread to main thread >-------
    private void clearDirs()
    {
      DirList.Items.Clear();
    }

    //----< function dispatched by child thread to main thread >-------
    private void addDir(string dir)
    {
      DirList.Items.Add(dir);
    }

    //----< function dispatched by child thread to main thread >-------
    private void insertParent()
    {
      DirList.Items.Insert(0, "..");
    }

    //----< function dispatched by child thread to main thread >-------
    private void clearFiles()
    {
      FileList.Items.Clear();
    }

    //----< function dispatched by child thread to main thread >-------
    private void clearConvertedFiles()
    {
      conFiles.Items.Clear();
    }

    //----< function dispatched by child thread to main thread >-------
    private void addFile(string file)
    {
      FileList.Items.Add(file);
    }

    //----< function dispatched by child thread to main thread >-------
    private void addConFile(string file)
    {
      conFiles.Items.Add(file);
    }

    //----< add client processing for message with key >---------------
    private void addClientProc(string key, Action<CsMessage> clientProc)
    {
      dispatcher_[key] = clientProc;
    }

    //----< load getDirs processing into dispatcher dictionary >-------
    private void DispatcherLoadGetDirs()
    {
      Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
      {
        Action clrDirs = () =>
        {
          clearDirs();
        };
        Dispatcher.Invoke(clrDirs, new Object[] { });
        var enumer = rcvMsg.attributes.GetEnumerator();
        while (enumer.MoveNext())
        {
          string key = enumer.Current.Key;
          if (key.Contains("dir"))
          {
            Action<string> doDir = (string dir) =>
            {
              addDir(dir);
            };
            Dispatcher.Invoke(doDir, new Object[] { enumer.Current.Value });
          }
        }
        Action insertUp = () =>
        {
          insertParent();
        };
        Dispatcher.Invoke(insertUp, new Object[] { });
      };
      addClientProc("getDirs", getDirs);
    }

    //----< load getFiles processing into dispatcher dictionary >------
    private void DispatcherLoadGetFiles()
    {
      Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
      {
        Action clrFiles = () =>
        {
          clearFiles();
        };
        Dispatcher.Invoke(clrFiles, new Object[] { });
        var enumer = rcvMsg.attributes.GetEnumerator();
        while (enumer.MoveNext())
        {
          string key = enumer.Current.Key;
          if (key.Contains("file"))
          {
            Action<string> doFile = (string file) =>
            {
              addFile(file);
            };
            Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
          }
        }
      };
      addClientProc("getFiles", getFiles);
    }

    //----< load publishFiles processing into dispatcher dictionary >------
    private void DispatcherPublishFiles()
    {
      Action<CsMessage> publishFiles = (CsMessage rcvMsg) =>
      {
        Action clrConvertedFiles = () =>
        {
          clearConvertedFiles();
        };
        Dispatcher.Invoke(clrConvertedFiles, new Object[] { });
        var enumer = rcvMsg.attributes.GetEnumerator();
        int conFilesCount = 0;
        while (enumer.MoveNext())
        {
          string key = enumer.Current.Key;
          if (key.Contains("conFile"))
          {
            Action<string> doFile = (string file) =>
            {
              addConFile(Path.GetFileName(file) + ".html");
              conFilesCount++;
              status.Content = "Status: Published " + conFilesCount + " files";
            };
            Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
          }
        }
      };
      addClientProc("publishFiles", publishFiles);
    }


    //----< load getConFiles processing into dispatcher dictionary >------
    private void DispatcherGetPublishedFiles()
    {
      Action<CsMessage> getConFiles = (CsMessage rcvMsg) =>
      {
        var enumer = rcvMsg.attributes.GetEnumerator();
        while (enumer.MoveNext())
        {
          string key = enumer.Current.Key;
          if (key.Contains("sentConFile"))
          {
            Action<string> doFile = (string file) =>
            {
              file = System.IO.Path.GetFullPath(workingDir + "/" + file);
              if(openOnce)
              {
                if(browserRadio.IsChecked == true)
                {
                  System.Diagnostics.Process.Start(file);
                }
                else
                {
                  swin = new SelectionWindow();
                  swin.setMainWindow(this);
                  swin.Show();
                  swin.Add(file);
                }
                openOnce = false;
              }
            };
            Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
          }
        }
      };
      addClientProc("getConFiles", getConFiles);
    }

    //----< load all dispatcher processing >---------------------------
    private void loadDispatcher()
    {
      DispatcherLoadGetDirs();
      DispatcherLoadGetFiles();
      DispatcherPublishFiles();
      DispatcherGetPublishedFiles();
    }

    //----< start Comm, fill window display with dirs and files >------
    private void Window_Loaded(object sender, RoutedEventArgs e)
    {
      //----< Receive the port and address information from the arguments and set it as the member variables >------
      string[] args = Environment.GetCommandLineArgs();
      string[] clientURLArray = args[1].Split(':');
      string[] serverURLArray = args[2].Split(':');
      clientAddress = clientURLArray[0];
      clientPort = int.Parse(clientURLArray[1]);
      serverAddress = serverURLArray[0];
      serverPort = int.Parse(serverURLArray[1]);
      if (!System.IO.Directory.Exists(workingDir))
        System.IO.Directory.CreateDirectory(workingDir);
      Patterns = "*.h *.cpp";
      Regex_ = "[A-Z](.*)";
      txtPatterns.Text = Patterns;
      txtRegex.Text = Regex_;
      // start Comm
      endPoint_ = new CsEndPoint();
      endPoint_.machineAddress = clientAddress;
      endPoint_.port = clientPort;
      translater = new Translater();
      translater.listen(endPoint_, workingDir, workingDir);
      processMessages();// start processing messages
      loadDispatcher();// load dispatcher
      CsEndPoint serverEndPoint = new CsEndPoint();
      serverEndPoint.machineAddress = serverAddress;
      serverEndPoint.port = serverPort;
      PathTextBlock.Text = "Storage";
      pathStack_.Push("../Storage");
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "getDirs");
      msg.add("path", pathStack_.Peek());
      translater.postMessage(msg);
      msg.remove("command");
      msg.add("command", "getFiles");
      translater.postMessage(msg);
      foreach (string arg in args)
      {
        if (arg == "/demo")
        {
          startDemonstration();
        }
      }
    }

    //----< send the server a request to publish all the files based on the pattern, regex and recurse fields >------
    private void publishFunction()
    {
      status.Content = "Status: ";
      CsEndPoint serverEndPoint = new CsEndPoint();
      serverEndPoint.machineAddress = serverAddress;
      serverEndPoint.port = serverPort;
      conFiles.Items.Clear();
      String arguments = null;
      List<String> conFilesList = new List<string>();
      arguments += "CodePublisher.exe" + " ";
      arguments += PathTextBlock.Text + " ";
      if ((bool)(cbRecurse.IsChecked))
      {
        arguments += "/s" + " ";
      }
      arguments += txtPatterns.Text + " ";
      arguments += txtRegex.Text + " ";
      CsMessage publishMsg = new CsMessage();
      publishMsg.add("args", arguments);
      publishMsg.add("to", CsEndPoint.toString(serverEndPoint));
      publishMsg.add("from", CsEndPoint.toString(endPoint_));
      publishMsg.add("command", "publishFiles");
      translater.postMessage(publishMsg);
    }

    //----< On the click of Publish button invokes the Native C++ publish function >---
    private void publishButton_Click(object sender, RoutedEventArgs e)
    {
      try
      {
        publishFunction();
      }
      catch (Exception ex)
      {
        MessageBox.Show("A handled exception just occurred: " + ex.Message, "Exception Sample", MessageBoxButton.OK, MessageBoxImage.Warning);
      }

    }

    //----< Starts the demonstraction if the argument contains demonstration >---
    private void startDemonstration()
    {
      Thread testThread = null; //Creates child thread which are needed for the Demo purpose
      testThread = new Thread(Executive_Test);
      testThread.Start();
    }


    //----< ATU for the Demo pupose. Automates all the GUI components >-------------
    public void Executive_Test()
    {
      Thread.Sleep(1500);
      Dispatcher.Invoke(() => { ATU_selectDirectory(); });
      Thread.Sleep(2000);
      Dispatcher.Invoke(() => { ATU_Publish(); });
      Thread.Sleep(3000);
      Dispatcher.Invoke(() => { ATU_ChangeTab(); });
      Thread.Sleep(1000);
      Dispatcher.Invoke(() => { ATU_GetConvertedFilesPopup(); });
      Thread.Sleep(3000);
      Dispatcher.Invoke(() => { ATU_GetConvertedFilesBrowser(); });
    }

    //----< Changes the Pattern, Regex, directory and Publishes for Demo >-------------
    public void ATU_selectDirectory()
    {
      Patterns = "*.h *.cpp";
      Regex_ = "[A-D](.*)";
      txtPatterns.Text = Patterns;
      txtRegex.Text = Regex_;
      string path;
      path = pathStack_.Peek() + "/" + "Test2";
      pathStack_.Push(path);
      PathTextBlock.Text = removeFirstDir(pathStack_.Peek());
      CsEndPoint serverEndPoint = new CsEndPoint();
      serverEndPoint.machineAddress = serverAddress;
      serverEndPoint.port = serverPort;
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "getDirs");
      msg.add("path", pathStack_.Peek());
      translater.postMessage(msg);
      msg.remove("command");
      msg.add("command", "getFiles");
      translater.postMessage(msg);
    }

    //----< Invokes the publish functions in the server with the default informations >-------------
    public void ATU_Publish()
    {
      publishFunction();
    }

    //----< Changes the tab of the Main window to the active window >-------------
    public void ATU_ChangeTab()
    {
      MyTabControl.SelectedIndex = 1;
    }

    //----< ATU to get the converted files from the server to the Local/Client Location 
    //      and display the published files in the default browser >-------------
    public void ATU_GetConvertedFilesBrowser()
    {
      browserRadio.IsChecked = true;
      string selStr = "Comm.cpp.html";
      openThePublishedFiles(selStr);
    }

    //----< ATU to get the converted files from the server to the Local/Client Location 
    //      and display the published files in the popup window >-------------
    public void ATU_GetConvertedFilesPopup()
    {
      popupRadio.IsChecked = true;
      string selStr = "Comm.cpp.html";
      openThePublishedFiles(selStr);
    }

    //----< strip off name of first part of path >---------------------
    private string removeFirstDir(string path)
    {
      string modifiedPath = path;
      int pos = path.IndexOf("/");
      modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
      return modifiedPath;
    }

    //----< respond to mouse double-click on dir name >----------------
    private void DirList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
    {
      // build path for selected dir
      string selectedDir = (string)DirList.SelectedItem;
      if (selectedDir == null)
        return;
      string path;
      if(selectedDir == "..")
      {
        if (pathStack_.Count > 1)  // don't pop off "Storage"
          pathStack_.Pop();
        else
          return;
      }
      else
      {
        path = pathStack_.Peek() + "/" + selectedDir;
        pathStack_.Push(path);
      }
      // display path in Dir TextBlock
      PathTextBlock.Text = removeFirstDir(pathStack_.Peek());
      // build message to get dirs and post it
      CsEndPoint serverEndPoint = new CsEndPoint();
      serverEndPoint.machineAddress = serverAddress;
      serverEndPoint.port = serverPort;
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "getDirs");
      msg.add("path", pathStack_.Peek());
      translater.postMessage(msg);
      // build message to get files and post it
      msg.remove("command");
      msg.add("command", "getFiles");
      translater.postMessage(msg);
    }

    private void openThePublishedFiles(string selStr)
    {
      string selStrFP = System.IO.Path.GetFullPath(workingDir + "/" + selStr);
      if (File.Exists(selStrFP))
      {
        if (browserRadio.IsChecked == true)
        {
          System.Diagnostics.Process.Start(selStrFP);
        }
        else
        {
          swin = new SelectionWindow();
          swin.setMainWindow(this);
          swin.Show();
          swin.Add(selStrFP);
        }
      }
      else
      {
        openOnce = true;
        CsEndPoint serverEndPoint = new CsEndPoint();
        serverEndPoint.machineAddress = serverAddress;
        serverEndPoint.port = serverPort;
        CsMessage getConFileMsg = new CsMessage();
        getConFileMsg.add("conFileName", selStr);
        getConFileMsg.add("to", CsEndPoint.toString(serverEndPoint));
        getConFileMsg.add("from", CsEndPoint.toString(endPoint_));
        getConFileMsg.add("command", "getConFiles");
        translater.postMessage(getConFileMsg);
      }
    }

    //----< open the files which are double-clicked in the default browser >---
    private void Con_MouseDoubleClick(object sender, MouseButtonEventArgs e)
    {
      string selStr = conFiles.SelectedItem.ToString();
      openThePublishedFiles(selStr);
    }
  }
}
