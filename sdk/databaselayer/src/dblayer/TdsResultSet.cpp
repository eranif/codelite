#include "../include/TdsResultSet.h"
#include "../include/TdsDatabaseLayer.h"
#include "../include/TdsResultSetMetaData.h"
#include "../include/DatabaseLayerException.h"
#include "../include/DatabaseErrorCodes.h"

// ctor
TdsResultSet::TdsResultSet(TDSSOCKET* pDatabase)
 : DatabaseResultSet()
{
  m_pDatabase = pDatabase;
  m_pResultSetMetaData = NULL;
  m_pResultInfo = NULL;
}

// dtor
TdsResultSet::~TdsResultSet()
{
  Close();
}


void TdsResultSet::Close()
{
  //fprintf(stderr, "TdsResultSet::Close()\n");
  CloseMetaData();

  //if (m_pResultInfo == NULL)
    //fprintf(stderr, "m_pResultInfo is still NULL!!!\n");
  
  if (m_pResultInfo != NULL)
    tds_free_results(m_pResultInfo);

  FreeResultSets();
}

void TdsResultSet::FreeResultSets()
{
  //fprintf(stderr, "In TdsResultSet::FreeResultSets()\n");
	int rc;
	int result_type;
	//while ((rc = tds_process_tokens(m_pDatabase, &result_type, NULL, TDS_TOKEN_RESULTS)) == TDS_SUCCEED)
	while ((rc = tds_process_tokens(m_pDatabase, &result_type, NULL, TDS_RETURN_ROW|TDS_TOKEN_RESULTS|TDS_RETURN_COMPUTE)) == TDS_SUCCEED)
	//while ((rc = tds_process_tokens(m_pDatabase, &result_type, NULL, TDS_TOKEN_RESULTS|TDS_RETURN_ROWFMT|TDS_RETURN_ROW|TDS_RETURN_COMPUTE)) == TDS_SUCCEED)
  {
		switch (result_type)
    {
		case TDS_DONE_RESULT:
		case TDS_DONEPROC_RESULT:
		case TDS_DONEINPROC_RESULT:
			/* ignore possible spurious result (TDS7+ send it) */
		case TDS_STATUS_RESULT:
			break;
		case TDS_ROWFMT_RESULT:
		case TDS_COMPUTEFMT_RESULT:
		case TDS_DESCRIBE_RESULT:
			break;
    case TDS_ROW_RESULT:
			//fprintf(stderr, "Warning:  TdsResultSet query should not return results.  Type: %d\n", result_type);
    /*
      if (m_pDatabase->current_results && m_pDatabase->current_results->num_cols > 0)
      {
			  fprintf(stderr, "Info:  TdsResultSet processing tokens\n");
  			while (tds_process_tokens(m_pDatabase, &result_type, NULL, TDS_RETURN_ROWFMT|TDS_RETURN_DONE|TDS_RETURN_ROW) == TDS_SUCCEED)
        {
		  	  fprintf(stderr, "Warning:  TdsResultSet TDS_ROW_RESULT query should not return results.  Type: %d\n", result_type);
			    if (result_type != TDS_ROW_RESULT)
			      break;
 
    			if (!m_pDatabase->current_results)
	      		continue;
		  	}
			  fprintf(stderr, "Info:  TdsResultSet done processing tokens\n");
      }
      else
      {
			  fprintf(stderr, "Info:  TdsResultSet NOT processing tokens\n");
      }
      if (m_pDatabase != NULL)
        tds_free_all_results(m_pDatabase);
      return;
      */
      if (m_pDatabase != NULL)
        tds_free_all_results(m_pDatabase);
      return;
			break;
		default:
			//fprintf(stderr, "Error:  TdsResultSet query should not return results.  Type: %d\n", result_type);
      // Clean up after ourselves
      if (m_pDatabase != NULL)
        tds_free_all_results(m_pDatabase);
			return;
		}
	}

  // Clean up after ourselves
  if (m_pDatabase != NULL)
    tds_free_all_results(m_pDatabase);

	if (rc == TDS_FAIL)
  {
		//fprintf(stderr, "tds_process_tokens() returned TDS_FAIL\n");
    SetErrorInformationFromDatabaseLayer();
    ThrowDatabaseException();
		return;
	}
  else if (rc != TDS_NO_MORE_RESULTS)
  {
		//fprintf(stderr, "tds_process_tokens() unexpected return\n");
    SetErrorInformationFromDatabaseLayer();
    ThrowDatabaseException();
		return;
	}
}


bool TdsResultSet::Next()
{
  //fprintf(stderr, "In TdsResultSet::Next()\n");
  int nResultType;
  int nReturn = tds_process_tokens(m_pDatabase, &nResultType, NULL, TDS_RETURN_ROWFMT|TDS_RETURN_ROW|TDS_RETURN_COMPUTE);

  //fprintf(stderr, "Result type: %d, Looking for %d\n", nResultType, TDS_ROWFMT_RESULT);
  if (nReturn == TDS_SUCCEED && nResultType == TDS_ROWFMT_RESULT)
  {
    PopulateFieldMap();
    CreateResultSetMetaData();
    // Go to the next row since the first call only returned the column names
    nReturn = tds_process_tokens(m_pDatabase, &nResultType, NULL, TDS_RETURN_ROWFMT|TDS_RETURN_ROW);//|TDS_RETURN_COMPUTE);
  }

  //fprintf(stderr, "tds_process_tokens returned %d (%d ?)\n", nReturn, TDS_SUCCEED);
  return (nReturn == TDS_SUCCEED);
}

void TdsResultSet::PopulateFieldMap()
{
  //fprintf(stderr, "In TdsResultSet::PopulateFieldMap()\n");

  TDSCOLUMN* curcol;
  //switch (nResultType)
  //{
  //  case TDS_ROW_RESULT:
      m_pResultInfo = m_pDatabase->res_info;
      for (int i = 0; i < m_pResultInfo->num_cols; i++)
      {
        curcol = m_pResultInfo->columns[i];
        char* pCharBuffer = new char[(curcol->column_namelen)+1];
        memset(pCharBuffer, 0, (curcol->column_namelen)+1);
        strncpy(pCharBuffer, curcol->column_name, curcol->column_namelen);
        wxString colName = ConvertFromUnicodeStream(pCharBuffer);
        wxDELETEA(pCharBuffer);
        //fprintf(stderr, "column name(%d) = '%s'\n", i, colName.c_str());
        m_FieldLookupMap[colName] = i;

        /*
        src = curcol->column_data;
        if (verbose)
        {
          srctype = curcol->column_type;
          srclen = curcol->column_size;
          tds_convert(&ctx, srctype, (TDS_CHAR *) src, srclen, SYBCHAR, &cr);
          printf("col %i is %s\n", i, cr.c);
        }
        if (i == 0)
        {
          src_id = *(int *) src;
        }
        else
        {
          src_val = *(float *) src;
          src_err = src_val - sybreal[src_id];
          if (src_err != 0.0)
          {
            src_err = src_err / src_val;
          }
          if (src_err < -tolerance || src_err > tolerance)
          {
            //fprintf(stderr, "SYBREAL expected %.8g  got %.8g\n",
            sybreal[src_id], src_val);
            //fprintf(stderr, "Error was %.4g%%\n", 100 * src_err);
            return 1;
          }
        }*/
      }
  //    row_count++;
  //  case TDS_COMPUTE_RESULT:
  //    break;
  //  default:
  //    fprintf(stderr, "tds_process_tokens() unexpected result\n");
  //    break;
  //}
}


// get field
int TdsResultSet::GetResultInt(int nField)
{
  return (int)GetResultLong(nField);
}

wxString TdsResultSet::GetResultString(int nField)
{
  TDSCOLUMN *curcol = m_pDatabase->current_results->columns[nField-1];
  TDS_CHAR *src = (TDS_CHAR *) curcol->column_data;

  wxString strValue = wxEmptyString;
  if (curcol->column_cur_size > 0)
  {
    char* pCharBuffer = new char[(curcol->column_cur_size)+1];
    memset(pCharBuffer, 0, (curcol->column_cur_size)+1);
    strncpy(pCharBuffer, src, curcol->column_cur_size);
    //fprintf(stderr, "char buffer: '%s'\n", pCharBuffer);
    strValue = ConvertFromUnicodeStream(pCharBuffer);
    //fprintf(stderr, "strValue: '%s'\n", strValue.c_str());
    wxDELETEA(pCharBuffer);
  }

  return strValue;
}

long TdsResultSet::GetResultLong(int nField)
{
  //fprintf(stderr, "In GetResultLong(%d)\n", nField);
  TDSCOLUMN* curcol = m_pDatabase->current_results->columns[nField-1];
  const int* src = (const int*) curcol->column_data;
  //fprintf(stderr, "%d\n", *src);

  long nValue = 0;
  wxString strValue = wxString::Format(_("%d"), *src);
  strValue.ToLong(&nValue);

  return nValue;
}

bool TdsResultSet::GetResultBool(int nField)
{
  long nValue = GetResultLong(nField);

  return (nValue != 0);
}

wxDateTime TdsResultSet::GetResultDate(int nField)
{
  // Don't use nField-1 here since GetResultString will take care of that
  TDSCOLUMN* curcol = m_pDatabase->current_results->columns[nField-1];
  TDS_DATETIME* dateTime = (TDS_DATETIME*) curcol->column_data;
  //fprintf(stderr, "Retrieved dtdays = %d, dttime = %d\n", dateTime->dtdays, dateTime->dttime);

  if (curcol->column_cur_size < 0)
    return wxInvalidDateTime;

  TDSDATEREC dateRec;
  TDS_INT nReturn = tds_datecrack(/*curcol->on_server.column_type*/SYBDATETIME, dateTime, &dateRec);
  if (nReturn == TDS_FAIL)
    return wxInvalidDateTime;

  //fprintf(stderr, "Second retrieved as %d\n", dateRec.second);

  wxDateTime date;
  date.Set(dateRec.day, wxDateTime::Month(dateRec.month), dateRec.year,
    dateRec.hour, dateRec.minute, dateRec.second, dateRec.millisecond);

  return date;
}

double TdsResultSet::GetResultDouble(int nField)
{
  TDSCOLUMN *curcol = m_pDatabase->current_results->columns[nField-1];
  const double* src = (const double*) curcol->column_data;

  double dblValue = *src;
  
  return dblValue;
}

void* TdsResultSet::GetResultBlob(int nField, wxMemoryBuffer& Buffer)
{
  TDSCOLUMN *curcol = m_pDatabase->current_results->columns[nField-1];
  TDS_CHAR *src = (TDS_CHAR *) curcol->column_data;

  if (curcol->column_cur_size < 0)
    return NULL;

  if (is_blob_type(curcol->column_type))
  {
    TDSBLOB *blob = (TDSBLOB*) src;

    src = blob->textvalue;
  }
  else
  {
    //fprintf(stderr, "Not a blob!!!\n");
    return NULL;
  }

  int nLength = curcol->column_cur_size;
  //fprintf(stderr, "Blob size = %d\n", nLength);
  void* pBuffer = Buffer.GetWriteBuf(nLength);
  memcpy(pBuffer, src, nLength);
  
  return Buffer.GetData();

/*
  CONV_RESULT cr;
  int conv_type = tds_get_conversion_type(curcol->column_type, curcol->column_size);
  if (tds_convert(test_context, conv_type, src, curcol->column_cur_size, SYBVARCHAR, &cr) < 0)
  {
    //fprintf(stderr, "Error converting\n");
    free(cr.c);
    return NULL;
  }
  else
  {
    if (strcmp(data[i_row].result, cr.c) != 0)
    {
      //fprintf(stderr, "Failed! Is \n%s\nShould be\n%s\n", cr.c, data[i_row].result);
      free(cr.c);
      return NULL;
    }
    free(cr.c);
  }

  return Buffer.GetData();
*/
}

bool TdsResultSet::IsFieldNull(int nField)
{
  TDSCOLUMN *curcol = m_pDatabase->current_results->columns[nField-1];
  if (curcol == NULL)
    return true;
  if (curcol->column_cur_size < 0)
    return true;

  return false;
}

int TdsResultSet::LookupField(const wxString& strField)
{
  StringToIntMap::iterator SearchIterator = m_FieldLookupMap.find(strField);
  if (SearchIterator == m_FieldLookupMap.end())
  {
    wxString msg(_("Field '") + strField + _("' not found in the resultset"));
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
    DatabaseLayerException error(DATABASE_LAYER_FIELD_NOT_IN_RESULTSET, msg);
    throw error;
#else
    wxLogError(msg);
#endif
    return -1;
  }
  else
  {
    return ((*SearchIterator).second + 1);  // Add +1 to make the result set 1-based rather than 0-based
  }
}

void TdsResultSet::CreateResultSetMetaData()
{
  m_pResultSetMetaData = new TdsResultSetMetaData(m_pResultInfo);
  LogMetaDataForCleanup(m_pResultSetMetaData);
}

ResultSetMetaData* TdsResultSet::GetMetaData()
{
  if (m_pResultSetMetaData == NULL)
  {
    // Just grab the result set info
    int nResultType;
    int nReturn = tds_process_tokens(m_pDatabase, &nResultType, NULL, TDS_RETURN_ROWFMT);

    if (nReturn == TDS_SUCCEED && nResultType == TDS_ROWFMT_RESULT)
    {
      PopulateFieldMap();
      CreateResultSetMetaData();
    }
  }

  return m_pResultSetMetaData;
}

void TdsResultSet::SetErrorInformationFromDatabaseLayer()
{
  TdsDatabaseLayer* pDatabaseLayer = TdsDatabaseLayer::LookupTdsLayer(m_pDatabase->tds_ctx);
  if (pDatabaseLayer != NULL)
  {
    SetErrorCode(pDatabaseLayer->GetErrorCode());
    SetErrorMessage(pDatabaseLayer->GetErrorMessage());
  }
}

