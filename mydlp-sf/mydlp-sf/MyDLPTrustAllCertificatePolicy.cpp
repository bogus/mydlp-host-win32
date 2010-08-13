#include "StdAfx.h"
#include "MyDLPTrustAllCertificatePolicy.h"

namespace mydlpsf {
	MyDLPTrustAllCertificatePolicy::MyDLPTrustAllCertificatePolicy(void)
	{
	}

	bool MyDLPTrustAllCertificatePolicy::CheckValidationResult(ServicePoint ^sp, X509Certificate ^cert,WebRequest ^req, int problem)
	{
		return true;
	}
}
