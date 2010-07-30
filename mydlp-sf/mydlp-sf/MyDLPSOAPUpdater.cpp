#include "StdAfx.h"
#include "MyDLPSOAPUpdater.h"
#include "MyDLPRemoteConf.h"
#include "MyDLPEventLogger.h"

using namespace System::Text;
using namespace System::Collections::Generic;

namespace mydlpsf
{
	MyDLPSOAPUpdater::MyDLPSOAPUpdater(void)
	{
	}

	MyDLPSOAPUpdater ^MyDLPSOAPUpdater::GetInstance()
	{
		if(obj == nullptr)
		{
			obj = gcnew MyDLPSOAPUpdater();

			obj->timer = gcnew System::Timers::Timer();
			obj->timer->Elapsed += gcnew ElapsedEventHandler(obj, &Update);
			obj->timer->Interval = 300000;
		}
		
		return obj;
	}

	void MyDLPSOAPUpdater::Update( Object ^source, ElapsedEventArgs ^e )
	{
		UpdateRules();
	}

	void MyDLPSOAPUpdater::StartUpdater()
	{
		UpdateRules();
		timer->Start();
	}
	
	void MyDLPSOAPUpdater::StopUpdater()
	{
		timer->Stop();
	}

	void MyDLPSOAPUpdater::UpdateRules()
	{
		try
		{
			if(MyDLPRemoteServiceConf::GetInstance()->isRemoteConfigUpdate.Equals(false))
				return;

			soap::MyDLPRuleVersion ^version = gcnew soap::MyDLPRuleVersion();
			version->Url = "http://"+MyDLPRemoteServiceConf::GetInstance()->remoteServer+"/mydlp-web-manager/service.php\?class=MyDLPRuleVersion";
			int ruleVer = version->getRuleVersion();

			if(ruleVer <= MyDLPRemoteServiceConf::GetInstance()->remoteRuleVersion)
				return;

			soap::MyDLPRuleManager ^manager = gcnew soap::MyDLPRuleManager();
			manager->Url = "http://"+MyDLPRemoteServiceConf::GetInstance()->remoteServer+"/mydlp-web-manager/service.php\?class=MyDLPRuleManager";
			soap::MyDLPRule ^rules =  manager->getRules();
			
			MyDLPRemoteDeviceConf ^deviceConf = MyDLPRemoteDeviceConf::GetInstance();
			deviceConf->excludedDirs = gcnew List<String ^>(rules->remoteDeviceConf->excludedDirs);
			deviceConf->enableRemovableOnlineScanning = rules->remoteDeviceConf->enableRemovableOnlineScanning;
			deviceConf->enableRemovableOnlineScanning_ruleid = rules->remoteDeviceConf->enableRemovableOnlineScanning_ruleid;
			deviceConf->justLogRemovableOnlineScanning = rules->remoteDeviceConf->justLogRemovableOnlineScanning;
			deviceConf->justLogRemovableOnlineScanning_ruleid = rules->remoteDeviceConf->justLogRemovableOnlineScanning_ruleid;
			deviceConf->scanInsertedLogical = rules->remoteDeviceConf->scanInsertedLogical;
			deviceConf->scanInsertedLogical_ruleid = rules->remoteDeviceConf->scanInsertedLogical_ruleid;
			deviceConf->scanPluggedInRemovableDevices = rules->remoteDeviceConf->scanPluggedInRemovableDevices;
			deviceConf->scanPluggedInRemovableDevices_ruleid = rules->remoteDeviceConf->scanPluggedInRemovableDevices_ruleid;
			deviceConf->filterCDR = rules->remoteDeviceConf->filterCDR;
			deviceConf->filterCDR_ruleid = rules->remoteDeviceConf->filterCDR_ruleid;
			deviceConf->filterDWG = rules->remoteDeviceConf->filterDWG;
			deviceConf->filterDWG_ruleid = rules->remoteDeviceConf->filterDWG_ruleid;
			deviceConf->filterPSD = rules->remoteDeviceConf->filterPSD;
			deviceConf->filterPSD_ruleid = rules->remoteDeviceConf->filterPSD_ruleid;
			deviceConf->filterPSP = rules->remoteDeviceConf->filterPSP;
			deviceConf->filterPSP_ruleid = rules->remoteDeviceConf->filterPSP_ruleid;

			MyDLPRemoteScreenCaptureConf ^scCapt = MyDLPRemoteScreenCaptureConf::GetInstance();
			scCapt->enableScreenCaptureFilter = rules->screenCaptureConf->enableScreenCaptureFilter;
			scCapt->enableScreenCaptureFilter_ruleid = rules->screenCaptureConf->enableScreenCaptureFilter_ruleid;
			scCapt->forbidAcrobatReader = rules->screenCaptureConf->forbidAcrobatReader;
			scCapt->forbidAcrobatReader_ruleid = rules->screenCaptureConf->forbidAcrobatReader_ruleid;
			scCapt->forbidAutoCAD = rules->screenCaptureConf->forbidAutoCAD;
			scCapt->forbidAutoCAD_ruleid = rules->screenCaptureConf->forbidAutoCAD_ruleid;
			scCapt->forbidMSOffice = rules->screenCaptureConf->forbidMSOffice;
			scCapt->forbidMSOffice_ruleid = rules->screenCaptureConf->forbidMSOffice_ruleid;
			scCapt->forbidOOOrg = rules->screenCaptureConf->forbidOOOrg;
			scCapt->forbidOOOrg_ruleid = rules->screenCaptureConf->forbidOOOrg_ruleid;
			scCapt->forbidPhotoshop = rules->screenCaptureConf->forbidPhotoshop;
			scCapt->forbidPhotoshop_ruleid = rules->screenCaptureConf->forbidPhotoshop_ruleid;

			MyDLPRemoteSensFileConf ^sensFile = MyDLPRemoteSensFileConf::GetInstance();
			sensFile->blockBroken = rules->sensFileConf->blockBroken;
			sensFile->blockBroken_ruleid = rules->sensFileConf->blockBroken_ruleid;
			sensFile->blockEncrypted = rules->sensFileConf->blockEncrypted;
			sensFile->blockEncrypted_ruleid = rules->sensFileConf->blockEncrypted_ruleid;
			sensFile->enableCC = rules->sensFileConf->enableCC;
			sensFile->enableCC_ruleid = rules->sensFileConf->enableCC_ruleid;
			sensFile->maxCCCount = rules->sensFileConf->maxCCCount;
			sensFile->enableTRId = rules->sensFileConf->enableTRId;
			sensFile->enableTRId_ruleid = rules->sensFileConf->enableTRId_ruleid;
			sensFile->maxTRIdCount = rules->sensFileConf->maxTRIdCount;
			sensFile->enableIBAN = rules->sensFileConf->enableIBAN;
			sensFile->enableIBAN_ruleid = rules->sensFileConf->enableIBAN_ruleid;
			sensFile->maxIBANCount = rules->sensFileConf->maxIBANCount;
			sensFile->enableSSN = rules->sensFileConf->enableSSN;
			sensFile->enableSSN_ruleid = rules->sensFileConf->enableSSN_ruleid;
			sensFile->maxSSNCount = rules->sensFileConf->maxSSNCount;
			
			sensFile->regexVal = gcnew List<MyDLPClamRegex ^>();
			for each (soap::MyDLPClamRegex ^regex in rules->sensFileConf->regexVal)
			{
				MyDLPClamRegex ^tmpRegex = gcnew MyDLPClamRegex();
				tmpRegex->id = regex->id;
				tmpRegex->name = regex->name;
				tmpRegex->regex = regex->regex;
				tmpRegex->rule_id = regex->rule_id;
				sensFile->regexVal->Add(tmpRegex);
			}

			sensFile->md5Val = gcnew List<MyDLPMD5File ^>();
			for each (soap::MyDLPMD5File ^file in rules->sensFileConf->md5Val)
			{
				MyDLPMD5File ^tmpFile = gcnew MyDLPMD5File();
				tmpFile->id = file->id;
				tmpFile->name = file->name;
				tmpFile->md5Val = file->md5Val;
				tmpFile->size = file->size;
				tmpFile->rule_id = file->rule_id;
				sensFile->md5Val->Add(tmpFile);
			}
			

			MyDLPRemoteScreenCaptureConf::Serialize();
			MyDLPRemoteSensFileConf::Serialize();
			MyDLPRemoteDeviceConf::Serialize();
			MyDLPRemoteServiceConf::GetInstance()->remoteRuleVersion = ruleVer;
			MyDLPRemoteServiceConf::Serialize();
			
		} catch(Exception ^ex) {
			MyDLPEventLogger::GetInstance()->LogError("MyDLPSOAPUpdater::UpdateRule " + ex->InnerException->StackTrace);
		}
	}

	String ^MyDLPSOAPUpdater::RuleToString(soap::MyDLPRule ^rule)
	{
		StringBuilder ^strBuilder = gcnew StringBuilder();

		strBuilder->Append("Sensitive file recognition values : \n");
		strBuilder->Append("\tMD5 File Values: \n");
		for each(soap::MyDLPMD5File ^file in rule->sensFileConf->md5Val)
		{
			strBuilder->Append("\t\t" + file->name + " rule: " + file->rule_id + "\n");
		}
		strBuilder->Append("\tRegular Expression Values: \n");
		for each(soap::MyDLPClamRegex ^regex in rule->sensFileConf->regexVal)
		{
			strBuilder->Append("\t\t" + regex->name + ", regex: "+ regex->regex +", rule: " + regex->rule_id + "\n");
		}
		strBuilder->Append("\tCredit card enabled: " + rule->sensFileConf->enableCC.ToString() + ", max count: " + rule->sensFileConf->maxCCCount.ToString() + "\n");
		strBuilder->Append("\tSSN enabled: " + rule->sensFileConf->enableSSN.ToString() + ", max count: " + rule->sensFileConf->maxSSNCount.ToString() + "\n");
		strBuilder->Append("\tTRid enabled: " + rule->sensFileConf->enableTRId.ToString() + ", max count: " + rule->sensFileConf->maxTRIdCount.ToString() + "\n");
		strBuilder->Append("\tIBAN enabled: " + rule->sensFileConf->enableIBAN.ToString() + ", max count: " + rule->sensFileConf->maxIBANCount.ToString() + "\n");
		
		strBuilder->Append("\tExcluded Directories: \n");
		for each(String ^dir in rule->remoteDeviceConf->excludedDirs)
			strBuilder->Append("\t\t" + dir + "\n");
		
		strBuilder->Append("\tEnable Removable Scan: " + rule->remoteDeviceConf->enableRemovableOnlineScanning.ToString() + "\n");
		strBuilder->Append("\tEnable Removable Scan - Just Log: " + rule->remoteDeviceConf->justLogRemovableOnlineScanning.ToString() + "\n");
		strBuilder->Append("\tScan removable on insert: " + rule->remoteDeviceConf->scanPluggedInRemovableDevices.ToString() + "\n");
		strBuilder->Append("\tScan logical on insert: " + rule->remoteDeviceConf->scanInsertedLogical.ToString() + "\n");

		strBuilder->Append("\tEnable screen capture filtering: " + rule->screenCaptureConf->enableScreenCaptureFilter.ToString() + "\n");

		return strBuilder->ToString();
	}

}