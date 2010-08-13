#pragma once

using namespace System::Security::Cryptography::X509Certificates;
using namespace System::Net;

namespace mydlpsf {

	public ref class MyDLPTrustAllCertificatePolicy : public System::Net::ICertificatePolicy
	{
	public:
		MyDLPTrustAllCertificatePolicy(void);
		virtual bool CheckValidationResult(ServicePoint ^sp, X509Certificate ^cert,WebRequest ^req, int problem);
	};

}
