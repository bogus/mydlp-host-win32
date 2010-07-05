using System;
using System.Collections.Generic;
using System.Text;
using Net.Sgoliver.NRtfTree.Core;
using Net.Sgoliver.NRtfTree.Util;
using System.IO;

namespace IFilterParser
{
    public class RTFParser
    {
        public void ParseRTF(String inputFilename, String outputFilename)
        {
            RtfTree tree = new RtfTree();
            tree.LoadRtfFile(inputFilename);
            StreamWriter writer = new StreamWriter(new FileStream(outputFilename, FileMode.OpenOrCreate), System.Text.Encoding.UTF8);
            writer.Write(tree.Text);
            writer.Close();
        }
    }
}
