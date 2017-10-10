<?php

function get_suggestions_form($form, $form_state) {
    //CSS laden
    drupal_add_css(drupal_get_path('module', 'extended_user_profile') . '/css/eup_custom.css', array('group' => CSS_DEFAULT));
    drupal_add_css(drupal_get_path('module', 'extended_user_profile') . '/css/eup_media.css', array('group' => CSS_DEFAULT));

    $form = array();

    $result = db_select('extended_user_profile_suggestions', 'ex')
        ->fields('ex', array('id', 'name'))
        ->condition('category', 'schools', '=')
        ->execute();

    $form['school_fieldset'] = array(
        '#type' => 'fieldset',
        '#title' => t('Vorgeschlagene Schulen'),
        '#tree' => TRUE,
    );

    $form['school_fieldset']['table_start'] = array(
        '#markup' => '<table><tr><th width="30px"></th><th>' . t('Vorschlag') . '</th><th width="110px">' . t('Löschen') . '</th></tr>'
    );

    $matches = array();

    $vocab = taxonomy_vocabulary_machine_name_load(variable_get('eup_school_machine_name'));
    $vid = $vocab->vid;

    $query = db_select('taxonomy_term_data', 'ttd');
    $query->fields('ttd', array('tid', 'name'));
    $query->join('taxonomy_term_hierarchy','tth' , 'ttd.tid = tth.tid');
    $query->condition('ttd.vid', $vid, '=');
    $query->condition('tth.parent', 0, '=');
    $community = $query->execute();

    $matches[SELECT_DEFAULT] = SELECT_DEFAULT;

    foreach ($community as $term) {
        $matches[check_plain($term -> tid)] = check_plain($term -> name);
    }


    $counter = 0;
    while ($data = $result->fetchAssoc()) {
        $form['school_fieldset']['schools'][$counter]['row_start'] = array(
            '#markup' => '<tr>'
        );

        $form['school_fieldset']['schools'][$counter]['school_check'] = array(
            '#type' => 'checkbox',
            '#prefix' => '<td>',
            '#suffix' => '</td>'
        );

        $form['school_fieldset']['schools'][$counter]['school_select'] = array(
            '#type' => 'select',
            '#options' => $matches,
            '#default_value' => SELECT_DEFAULT,
            '#prefix' => '<td><div class="left-floated-element">',
            '#suffix' => '</div>',
        );

        $form['school_fieldset']['schools'][$counter]['school_input'] = array(
            '#type' => 'textfield',
            '#default_value' => $data['name'],
            '#prefix' => '<div class="left-floated-element">',
            '#suffix' => '</div><div class="clear-float"></div></td>',
        );

        $form['school_fieldset']['schools'][$counter]['school_delete'] = array(
            '#type' => 'submit',
            '#name' => 'del_buttn_' . $data['id'],
            '#value' => t('Löschen'),
            '#submit' => array('delete_suggest_school_submit'),
            '#attributes' => array('suggest-id' => $data['id']),
            '#prefix' => '<td>',
            '#suffix' => '</td>',
        );

        $form['school_fieldset']['schools'][$counter]['suggest_id'] = array(
            '#type' => 'hidden',
            '#value' => $data['id'],
        );

        $form['school_fieldset']['schools'][$counter]['row_end'] = array(
            '#markup' => '</tr>'
        );

        $counter++;
    }

    $form['school_fieldset']['table_end'] = array(
        '#markup' => '</table>'
    );

    $form['save_button'] = array(
        '#type' => 'submit',
        '#value' => t('Speichern'),
        '#submit' => array('save_suggest_school_submit'),
        '#validate' => array('save_suggest_school_validate'),

    );

    return $form;
}

function save_suggest_school_validate($form, &$form_state) {


    $index = 0;
    foreach ($form_state['values']['school_fieldset']['schools'] as $value) {
        if ($value['school_check'] == 1 && $value['school_select'] == SELECT_DEFAULT) {
            form_set_error('school_fieldset][schools][' . $index . '][school_select', t('Bei Ausgewählten Vorschlägen eine Gemeinde definieren.'));
        }

        $index++;
    }
}

function save_suggest_school_submit($form, &$form_state) {

    $vocab = taxonomy_vocabulary_machine_name_load(variable_get('eup_school_machine_name'));
    $vid = $vocab->vid;

    foreach ($form_state['values']['school_fieldset']['schools'] as $value) {
        if ($value['school_check'] == 1) {
            $term = new stdClass();
            $term->name = $value['school_input'];
            $term->vid = $vid;
            $term->parent = $value['school_select'];
            taxonomy_term_save($term);


            db_delete('extended_user_profile_suggestions')
                ->condition('id', $value['suggest_id'], '=')
                ->execute();
        }
    }

    drupal_set_message(t('Gespeichert'));
}

function delete_suggest_school_submit($form, &$form_state) {
    $id = $form_state['triggering_element']['#attributes']['suggest-id'];

    db_delete('extended_user_profile_suggestions')
        ->condition('id', $id, '=')
        ->execute();

    drupal_set_message(t('Vorschlag wurde gelöscht.'));
}

function suggest_school_form($form, &$form_state) {

    //CSS laden
    drupal_add_css(drupal_get_path('module', 'extended_user_profile') . '/css/eup_custom.css', array('group' => CSS_DEFAULT));
    drupal_add_css(drupal_get_path('module', 'extended_user_profile') . '/css/eup_media.css', array('group' => CSS_DEFAULT));

    $form = array();

    $form['school_input'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Folgende Schule schlage ich vor'),
        '#default_value' => '',
    );

    $form['suggest_school_submit'] = array(
        '#type' => 'submit',
        '#value' => t('Vorschlag abschicken'),
        '#submit' => array('suggest_input_submit'),
    );
    return $form;
}

function suggest_school_form_validate($form, $form_state) {
    if (empty($form_state['values']['school_input'])) {
        form_set_error('school_input', t('Leerer Vorschlag kann nicht abgeschickt werden.'));
    }
}

function suggest_input_submit($form, &$form_state) {

    $result = db_select('extended_user_profile_suggestions', 'ex')
        ->fields('ex', array('id'))
        ->orderBy('id', 'DESC')
        ->execute()
        ->fetchAssoc();

    $id = 0;

    if ($result) {
        $id = $result['id'];
        $id++;
    }

    db_insert('extended_user_profile_suggestions')
        ->fields(array(
            'id' => $id,
            'category' => 'schools',
            'name' => $form_state['values']['school_input'],
        ))
        ->execute();
    drupal_set_message(t('Vorschlag wurde abgeschickt.'));
}