/*
 *  Reference: http://www.eggheadcafe.com/tutorials/aspnet/9ce6c242-c14c-4969-9251-af95e4cf320f/zip--unzip-folders-and-f.aspx
 *  Author: Peter Bromberg
 * 
 */
using System;
using System.Collections;
using System.IO;
using ICSharpCode.SharpZipLib.Zip;
using Microsoft.Win32;
using System.ServiceProcess;

namespace MydlpWinGui
{
    public static class ZipUtil
    {
        public static void ZipFiles(string outputPathAndFile)
        {
            string inputFolderPath = (string)Registry.LocalMachine.OpenSubKey("Software\\MyDLP").GetValue("Config_Dir");
            ArrayList ar = GenerateFileList(inputFolderPath); // generate file list
            int TrimLength = (Directory.GetParent(inputFolderPath)).ToString().Length;
            // find number of chars to remove     // from orginal file path
            TrimLength += 1; //remove '\'
            FileStream ostream;
            byte[] obuffer;
            string outPath = outputPathAndFile;
            ZipOutputStream oZipStream = new ZipOutputStream(File.Create(outPath)); // create zip stream
            oZipStream.SetLevel(9); // maximum compression
            ZipEntry oZipEntry;
            foreach (string Fil in ar) // for each file, generate a zipentry
            {
                oZipEntry = new ZipEntry(Fil.Remove(0, TrimLength));
                oZipStream.PutNextEntry(oZipEntry);

                if (!Fil.EndsWith(@"/")) // if a file ends with '/' its a directory
                {
                    ostream = File.OpenRead(Fil);
                    obuffer = new byte[ostream.Length];
                    ostream.Read(obuffer, 0, obuffer.Length);
                    oZipStream.Write(obuffer, 0, obuffer.Length);
                }
            }
            oZipStream.Finish();
            oZipStream.Close();
        }


        private static ArrayList GenerateFileList(string Dir)
        {
            ArrayList fils = new ArrayList();
            bool Empty = true;
            foreach (string file in Directory.GetFiles(Dir)) // add each file in directory
            {
                fils.Add(file);
                Empty = false;
            }

            if (Empty)
            {
                if (Directory.GetDirectories(Dir).Length == 0)
                // if directory is completely empty, add it
                {
                    fils.Add(Dir + @"/");
                }
            }

            foreach (string dirs in Directory.GetDirectories(Dir)) // recursive
            {
                foreach (object obj in GenerateFileList(dirs))
                {
                    fils.Add(obj);
                }
            }
            return fils; // return file list
        }


        public static void UnZipFiles(string zipPathAndFile)
        {
            string outputFolder = (string)Registry.LocalMachine.OpenSubKey("Software\\MyDLP").GetValue("Config_Dir");
            string originalFolder = outputFolder;
            string backupFolder = outputFolder + "-bak";

            foreach (ServiceController sc in ServiceController.GetServices())
            {
                if (sc.ServiceName == "MyDLP-Host")
                {
                    if (sc.Status == ServiceControllerStatus.Running)
                        sc.Stop();
                }
            }

            CopyAll(new DirectoryInfo(outputFolder), new DirectoryInfo(backupFolder));

            try
            {
                if (Directory.Exists(backupFolder))
                    Directory.Delete(backupFolder, true);

                Directory.Delete(outputFolder, true);

                outputFolder = Directory.GetParent(outputFolder).FullName;

                ZipInputStream s = new ZipInputStream(File.OpenRead(zipPathAndFile));
                ZipEntry theEntry;
                string tmpEntry = String.Empty;
                while ((theEntry = s.GetNextEntry()) != null)
                {
                    string directoryName = outputFolder;
                    string fileName = Path.GetFileName(theEntry.Name);
                    // create directory 
                    if (directoryName != "")
                    {
                        Directory.CreateDirectory(directoryName);
                    }
                    if (fileName != String.Empty)
                    {
                        if (theEntry.Name.IndexOf(".ini") < 0)
                        {
                            string fullPath = directoryName + "\\" + theEntry.Name;
                            fullPath = fullPath.Replace("\\ ", "\\");
                            string fullDirPath = Path.GetDirectoryName(fullPath);
                            if (!Directory.Exists(fullDirPath)) Directory.CreateDirectory(fullDirPath);
                            FileStream streamWriter = File.Create(fullPath);
                            int size = 2048;
                            byte[] data = new byte[2048];
                            while (true)
                            {
                                size = s.Read(data, 0, data.Length);
                                if (size > 0)
                                {
                                    streamWriter.Write(data, 0, size);
                                }
                                else
                                {
                                    break;
                                }
                            }
                            streamWriter.Close();
                        }
                    }
                }
                s.Close();
            }
            catch (Exception ex)
            {
                if(Directory.Exists(originalFolder))
                    Directory.Delete(originalFolder, true);
                CopyAll(new DirectoryInfo(backupFolder), new DirectoryInfo(originalFolder));

                throw new Exception("Error in unzip",ex);
            }
            finally
            {
                if (Directory.Exists(backupFolder))
                    Directory.Delete(backupFolder, true);
                foreach (ServiceController sc in ServiceController.GetServices())
                {
                    if (sc.ServiceName == "MyDLP-Host")
                    {
                        if (sc.Status != ServiceControllerStatus.Running)
                            sc.Start();
                    }
                }
            }
        }

        public static void CopyAll(DirectoryInfo source, DirectoryInfo target)
        {
            // Check if the target directory exists, if not, create it.
            if (Directory.Exists(target.FullName) == false)
            {
                Directory.CreateDirectory(target.FullName);
            }

            // Copy each file into it’s new directory.
            foreach (FileInfo fi in source.GetFiles())
            {
                fi.CopyTo(Path.Combine(target.ToString(), fi.Name), true);
            }

            // Copy each subdirectory using recursion.
            foreach (DirectoryInfo diSourceSubDir in source.GetDirectories())
            {
                DirectoryInfo nextTargetSubDir =
                    target.CreateSubdirectory(diSourceSubDir.Name);
                CopyAll(diSourceSubDir, nextTargetSubDir);
            }
        }
    }
}