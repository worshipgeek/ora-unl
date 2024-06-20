SELECT
	SUBSTR(wonum,1,5),
	SUBSTR(TO_CHAR(wpoperation,'000'),2,3),
	TO_CHAR(work_date,'YYYYMMDD')
FROM
	wdw_jc_labor;
