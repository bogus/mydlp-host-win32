// requires Windows 2000 Service Pack 4, Windows Server 2003, Windows XP Service Pack 2
// requires KB 835732 on Windows 2000 Service Pack 4
// http://www.microsoft.com/downloads/details.aspx?FamilyID=79bc3b77-e02c-4ad3-aacf-a7633f706ba5

[CustomMessages]
vcredist2008_title=Microsoft Visual C++ 2008 Redistributable Package

en.vcredist2008_size=1.7 - 4.0 MB
de.vcredist2008_size=1,7 - 4,0 MB


[Code]	
const
	vcredist2008_url = 'http://download.microsoft.com/download/1/1/1/1116b75a-9ec3-481a-a3c8-1777b5381140/vcredist_x86.exe';
	vcredist2008_url_x64 = 'http://download.microsoft.com/download/d/2/4/d242c3fb-da5a-4542-ad66-f9661d0a8d19/vcredist_x64.exe';
	vcredist2008_url_ia64 = 'http://download.microsoft.com/download/a/1/a/a1a4996b-ed78-4c2b-9589-8edd81b8df39/vcredist_IA64.exe';

procedure vcredist2008();
var
	version: cardinal;
begin
	// RegQueryDWordValue(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{9A25302D-30C0-39D9-BD6F-21E6EC160475}', 'VersionMajor', version);
	if not RegKeyExists(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{9A25302D-30C0-39D9-BD6F-21E6EC160475}') and
    not RegKeyExists(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{8220EEFE-38CD-377E-8595-13398D740ACE}') and
      not RegKeyExists(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{5827ECE1-AEB0-328E-B813-6FC68622C1F9}') then
	     AddProduct('vcredist2008.exe',
  			 '',
			   CustomMessage('vcredist2008_title'),
	   		 CustomMessage('vcredist2008_size'),
		     GetURL(vcredist2008_url, vcredist2008_url_x64, vcredist2008_url_ia64));
end;
