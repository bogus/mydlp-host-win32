using System;
using System.Collections.Generic;
using System.Text;
using org.pdfbox.pdmodel;
using org.pdfbox.util;

namespace IFilterParser
{
    public class PDFParser
    {
        public String parseUsingPDFBox(String input)
        {
            PDDocument doc = PDDocument.load(input);
            PDFTextStripper stripper = new PDFTextStripper();
            return stripper.getText(doc);
        }
    }
}
