<?php

function set_ids_form($form, &$form_state) {
    $form = array();

    //CSS laden
    drupal_add_css(drupal_get_path('module', 'extended_user_profile') . '/css/eup_custom.css', array('group' => CSS_DEFAULT));
    drupal_add_css(drupal_get_path('module', 'extended_user_profile') . '/css/eup_media.css', array('group' => CSS_DEFAULT));

    $form = array();

    $form['school'] = array(
        '#type'  => 'fieldset',
        '#title' => t('Schulen'),
    );

    $form['school']['eup_school_machine_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Systemname der Taxonomie für Schulen'),
        '#default_value' => variable_get('eup_school_machine_name', ''),
    );

    $form['school']['eup_jfproject_machine_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Systemname der Taxonomie für Jugend Forscht Projekte'),
        '#default_value' => variable_get('eup_jfproject_machine_name', ''),
    );

    $form = system_settings_form($form);

    return $form;
}