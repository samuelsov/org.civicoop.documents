<?php

/**
 * This class checks for any upload fields on 
 * the webform and saves those files into
 * the civicrm document store
 */
class WebformCiviCRMDocuments {

  private $submission;
  private $node;
  private $caseId = false;

  /**
   * 
   * @param int $nid - Node ID of the webform
   * @param int $sid - ID of the submitted result
   */
  public function __construct($nid, $sid) {
    $this->node = node_load($nid);
    $this->submission = webform_get_submission($nid, $sid);
  }

  public function saveDocuments() {
    if (!$this->isValidSubmission()) {
      return;
    }

    foreach ($this->node->webform['components'] as $cid => $field) {
      if ($this->checkComponentForDocument($cid, $field)) {
        $this->saveFiles($field['name'], $this->submission->data[$cid], $field);
      }
    }
  }
  
  /**
   * Returns what to do when a document exist
   * e.g. create a new document or create a new version of the document
   * 
   * @param type $component
   * @return string
   */
  protected function getExistingAction($component) {
    $existing = 'new_version';
    if (isset($component['extra']['webform_civicrm_documents_existing'])) {
      if ($component['extra']['webform_civicrm_documents_existing'] == 'new_document') {
        $existing = 'new_document';
      }
    }
    return $existing;
  }
  
  /**
   * Returns if this component is a valid field for adding documents to civicrm
   * 
   * @param type $cid
   * @param type $component
   * @return boolean
   */
  protected function checkComponentForDocument($cid, $component) {
    if ($component['type'] != 'file') {
      return false;
    }
    if (!isset($this->submission->data[$cid])) {
      return false;
    }
    
    if (!isset($component['extra']['webform_civicrm_documents_enabled'])) {
      return false;
    }
    if (!$component['extra']['webform_civicrm_documents_enabled']) {
      return false;
    }
    return true;
  }
  
  /**
   * Checks if the submitted data is linked to civicrm
   */
  protected function isValidSubmission() {
    if (!isset($this->node->webform)) {
      return false;
    }

    if (!isset($this->node->webform['components'])) {
      return false;
    }

    //do a check if this is a civicrm submission
    if (!isset($this->submission->civicrm)) {
      return false;
    }
    
    if (!isset($this->submission->civicrm['contact_id'])) {
      return false;
    }
    
    if (count($this->submission->civicrm['contact_id']) == 0) {
      return false;
    }
    
    $this->caseId = false;
    if (isset($this->submission->civicrm['activity_id'])) {
      $caseActivity = new CRM_Case_DAO_CaseActivity();
      $caseActivity->activity_id = $this->submission->civicrm['activity_id']; 
      if ($caseActivity->find(TRUE)) {
         $this->caseId = $caseActivity->case_id;
      }
    }
    
    return true;
  }

  protected function saveFiles($subject, $submission, $component) {
    if (count($submission) == 1) {
      //single file
      $fid = reset($submission);
      $this->saveFileAsDocument($fid, $subject, $component);
    } elseif (count($submission) > 1) {
      //multiple files
      $i = 1;
      foreach ($submission as $fid) {
        $this->saveFileAsDocument($fid, $subject.' '.$i, $component);
        $i ++;
      }
    }
  }
  
  protected function saveFileAsDocument($fid, $subject, $component) {
    $documentsRepo = CRM_Documents_Entity_DocumentRepository::singleton();
       
    $file = file_load($fid);
    $wrapper = file_stream_wrapper_get_instance_by_uri($file->uri);
    $filepath = $wrapper->realpath();
        
    $doc = $this->createDocument($subject, $component);
    $documentsRepo->persist($doc);
    
    CRM_Documents_Utils_File::copyFileToDocument($filepath, $file->filemime, $doc);
  }
  
  /**
   * Returns a new or an existing document
   */
  protected function createDocument($subject, $component) {
    $existingAction = $this->getExistingAction($component);
    if ($existingAction == 'new_document') {
      $doc = new CRM_Documents_Entity_Document();
    } else {
      $doc = $this->findDocument($subject);
    }
    
    foreach($this->submission->civicrm['contact_id'] as $contact_id) {
      $doc->addContactId($contact_id);
    }
    
    if ($this->caseId) {
      $doc->addCaseId($this->caseId);
    }
    
    $doc->setSubject($subject);
    $version = $doc->addNewVersion();
    $version->setDescription('Webform upload from: '.$subject);
    return $doc;
  }
  
  /**
   * Find a document based on the parameters given
   * 
   * Returns false when no document is found
   */
  protected function findDocument($subject) {
    $repo = CRM_Documents_Entity_DocumentRepository::singleton();
    
    if ($this->caseId) {
      $docs = $repo->getDocumentsByCaseIdAndSubject($this->caseId, $subject);
      if (count($docs) == 1) {
        return reset($docs); //return the first document
      }
    } else {
      foreach($this->submission->civicrm['contact_id'] as $contact_id) {
        $docs = $repo->getDocumentsByContactIdAndSubject($contact_id, $subject);
        if (count($docs) == 1) {
          return reset($docs); //return the first document
        }
      }
    }
    
    return new CRM_Documents_Entity_Document();
  }

}