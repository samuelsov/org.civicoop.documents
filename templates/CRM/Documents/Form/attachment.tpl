{*

*}
{if $form.attachFile_1 OR $currentAttachmentInfo}

    {capture assign=attachTitle}{ts}Attachment(s){/ts}{/capture}

    <div id="attachments">
      <table class="form-layout-compressed">
      {if $form.attachFile_1}
        <tr>
          <td class="label">{$form.attachFile_1.label}</td>
          <td>{$form.attachFile_1.html}&nbsp;<span class="crm-clear-link">(<a href="#" onclick="clearAttachment( '#attachFile_1', '#attachDesc_1' ); return false;">{ts}clear{/ts}</a>)</span><br />
            <span class="description">{ts}Browse to the <strong>file</strong> you want to upload.{/ts}{if $maxAttachments GT 1} {ts 1=$maxAttachments}You can have a maximum of %1 attachment(s).{/ts}{/if} Each file must be less than {$config->maxFileSize}M in size. You can also add a short description.</span>
          </td>
        </tr>
        {if $form.tag_1.html}
          <tr>
            <td></td>
            <td><label>{$form.tag_1.label}</label> <div class="crm-select-container crm-attachment-tags">{$form.tag_1.html}</div></td>
          </tr>
        {/if}
        {if $tagsetInfo_attachment}
          <tr><td></td><td>{include file="CRM/common/Tag.tpl" tagsetType='attachment' tagsetNumber=1 }</td></tr>
        {/if}
        {section name=attachLoop start=2 loop=$numAttachments+1}
          {assign var=index value=$smarty.section.attachLoop.index}
          {assign var=attachName value="attachFile_"|cat:$index}
          {assign var=attachDesc value="attachDesc_"|cat:$index}
          {assign var=tagElement value="tag_"|cat:$index}
            <tr class="attachment-fieldset"><td colspan="2"></td></tr>
            <tr>
                <td class="label">{$form.attachFile_1.label}</td>
                <td>{$form.$attachName.html}&nbsp;<span class="crm-clear-link">(<a href="#" onclick="clearAttachment( '#{$attachName}' ); return false;">{ts}clear{/ts}</a>)</span></td>
            </tr>
            <tr>
              <td></td>
              <td><label>{$form.$tagElement.label}</label> <div class="crm-select-container crm-attachment-tags">{$form.$tagElement.html}</div></td>
            </tr>
            {if $tagsetInfo_attachment}
              <tr><td></td><td>{include file="CRM/common/Tag.tpl" tagsetType='attachment' tagsetNumber=$index}</td></tr>
            {/if}
        {/section}

        {literal}
          <script type="text/javascript">
            cj(".crm-attachment-tags select[multiple]").crmasmSelect({
              addItemTarget: 'bottom',
              animate: true,
              highlight: true,
              sortable: true,
              respectParents: true
            });
          </script>
        {/literal}
      {/if}
      {if $currentAttachmentInfo}
        <tr class="attachment-fieldset"><td colspan="2"></td></tr>
        <tr>
            <td class="label">{ts}Current Attachment(s){/ts}</td>
            <td class="view-value">
          {foreach from=$currentAttachmentInfo key=attKey item=attVal}
                <div id="attachStatusMesg" class="status hiddenElement"></div>
                <div id="attachFileRecord_{$attVal.fileID}">
                  <strong><a href="{$attVal.url}">{$attVal.cleanName}</a></strong>
                  {if $attVal.description}&nbsp;-&nbsp;{$attVal.description}{/if}
                  {if $attVal.deleteURLArgs}
                   <a href="#" onclick="showDeleteAttachment('{$attVal.cleanName}', '{$attVal.deleteURLArgs}', {$attVal.fileID}, '#attachStatusMesg', '#attachFileRecord_{$attVal.fileID}'); return false;" title="{ts}Delete this attachment{/ts}"><span class="icon red-icon delete-icon" style="margin:0px 0px -5px 20px" title="{ts}Delete this attachment{/ts}"></span></a>
                  {/if}
                  {if !empty($attVal.tag)}
                    <br/>
                    {ts}Tags{/ts}: {$attVal.tag}
                    <br/>
                  {/if}
                </div>
          {/foreach}
            </td>
        </tr>
        <tr>
            <td class="label">&nbsp;</td>
            <td>{$form.is_delete_attachment.html}&nbsp;{$form.is_delete_attachment.label}<br />
                <span class="description">{ts}Click the red trash-can next to a file name to delete a specific attachment. If you want to delete ALL attachments, check the box above and click Save.{/ts}</span>
            </td>
        </tr>
      {/if}
      </table>
    </div>
  </div><!-- /.crm-accordion-body -->
  </div><!-- /.crm-accordion-wrapper -->
  {if !$noexpand}
    {literal}
    <script type="text/javascript">
    cj(function() {
       cj().crmAccordions();
    });
    </script>
    {/literal}
  {/if}
    {literal}
    <script type="text/javascript">
      function clearAttachment( element, desc ) {
        cj(element).val('');
        cj(desc).val('');
      }
    </script>
    {/literal}

{if $currentAttachmentInfo}
{include file="CRM/Form/attachmentjs.tpl"}
{/if}

{/if} {* top level if *}

