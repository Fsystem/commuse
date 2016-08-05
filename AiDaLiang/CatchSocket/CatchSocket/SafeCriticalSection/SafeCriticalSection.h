#pragma once

namespace COMMUSE
{
	/*****************************************************************************
	* ��  ����CCriticalSection
	* ��  �ܣ�����
	* ��  �ߣ�˫�н�
	* ��  �ڣ�2010-12-29 9:21:12
	******************************************************************************/
	#define LOCK(cs) CCritiSection::CLock lk(cs)

	class CCritiSection
	{
	public:
		class CLock
		{
		public:

			CLock(CCritiSection& cs):_crit(cs){  _crit.Enter();  };

			virtual ~CLock(){  _crit.Leave();  };

		protected:
		private:
			CCritiSection& _crit;
		};

		CCritiSection();

		virtual ~CCritiSection();

		void Enter(){ EnterCriticalSection(&crit); };

		void Leave(){ LeaveCriticalSection(&crit); };

	protected:
	private:
		CRITICAL_SECTION crit;
	};



}