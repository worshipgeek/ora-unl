SELECT
	SUBSTR(wonum,1,5),
	TO_CHAR(wpoperation,'000'),
	TO_CHAR(work_date,'YYYYMMDD')
FROM
	wdw_jc_labor;
