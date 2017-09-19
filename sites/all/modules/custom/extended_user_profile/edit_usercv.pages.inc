<?php

function extended_user_profile_cv_form($form, &$form_state, $user_id) {

    $userLoad = user_load($user_id);
    if (!$userLoad) {
        drupal_not_found();
        drupal_exit();
    }

    global $user;

    if ($user_id != $user->uid && !user_has_role(3, user_load($user->uid))) {
        drupal_access_denied();
        drupal_exit();
    }

    $fields = array(
        'image_id',
        'salutation',
        'title',
        'first_name',
        'last_name',
        'birthday_day',
        'birthday_month',
        'birthday_year',
        'email',
        'city',
        'zip_code',
        'telephone',
        'practical_experience',
        'education',
        'language_skills',
        'special_knowledge'
    );

    $user_id = ($user_id != $user->uid && user_has_role(3, user_load($user->uid))) ? $user_id : $user->uid;
    $result = db_select('extended_user_profile_cv', 'ex')
        ->fields('ex', $fields)
        ->condition('user_id', $user_id, '=')
        ->execute()
        ->fetchAssoc();

    $form['profile_user_id'] = array(
        '#type' => 'hidden',
        '#value' => $user_id,
    );

    $form['personal'] = array(
        '#type' => 'fieldset',
        '#title' => t('Persönliches'),
        '#tree' => TRUE,
    );

    if ($result['image_id'] != 0) {

        $image = file_load($result['image_id']);

        $form['personal']['profile_image'] = array(
            '#markup' => '<img src="' . file_create_url($image->uri) . '" width="200px"><br>'
        );

        $form['personal']['delete_image'] = array(
            '#type' => 'submit',
            '#value' => t('Bild löschen'),
            '#submit' => array('delete_image_button_submit'),
        );

        $form['personal']['profile_image_id'] = array(
            '#type' => 'hidden',
            '#value' => $result['image_id'],
        );
    }

    $form['personal']['profile_image_upload'] = array(
        '#type' => 'file',
        '#title' => t('Profilbild hochladen'),
        '#description' => t('Erlaubte Erweiterungen: jpg, jpeg, png, gif'),
    );


    $form['personal']['firstname'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Vorname'),
        '#size' => 20,
        '#default_value' => (isset($result['first_name']) && !empty($result['first_name'])) ? $result['first_name'] : '',
    );

    $form['personal']['lastname'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Nachname'),
        '#size' => 20,
        '#default_value' => (isset($result['last_name']) && !empty($result['last_name'])) ? $result['last_name'] : '',
    );

    $form['personal']['birthday'] = array(
        '#type' => 'date',
        '#title' => t('Geburtsdatum'),
        '#default_value' => (isset($result['birthday_day'])) ? array('year' => $result['birthday_year'], 'month' => $result['birthday_month'], 'day' => $result['birthday_day']) : array('year' => 1990, 'month' => 1, 'day' => 1),
    );

    $form['personal']['email'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('E-Mail'),
        '#size' => 20,
        '#prefix' => '<br><hr><br>',
        '#default_value' => (isset($result['first_name']) && !empty($result['first_name'])) ? $result['first_name'] : '',
    );

    $form['personal']['city'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Wohnort'),
        '#size' => 20,
        '#default_value' => (isset($result['city']) && !empty($result['city'])) ? $result['city'] : '',
    );

    $form['personal']['zip_code'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('PLZ'),
        '#size' => 20,
        '#default_value' => (isset($result['zip_code']) && $result['zip_code'] != 0) ? $result['zip_code'] : '',
    );

    $form['personal']['telephone'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Telefon/Mobil'),
        '#size' => 20,
        '#default_value' => (isset($result['telephone']) && !empty($result['telephone'])) ? $result['telephone'] : '',
    );



    $form['practical_experience'] = array(
        '#type' => 'fieldset',
        '#title' => t('Praxiserfahrungen'),
        '#prefix' => '<div id="replace-practical-experience">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    // Build the initial fields if this is not being rebuilt
    // from ajax request.
    if (!array_key_exists('clicked_button', $form_state)) {
        if (isset($result['practical_experience'])) {
            $practicalExperience = unserialize($result['practical_experience']);
            if (count($practicalExperience) != 0) {
                $index = 0;

                foreach ($practicalExperience as $key => $value) {
                    create_practical_experience($form, $index, $practicalExperience);
                    $index++;
                }

            } else {
                create_practical_experience($form, 0, array(''));
            }

        } else {
            create_practical_experience($form, 0, array(''));
        }
    } else {


        // Otherwise add the fields for each existing value.
        foreach ($form_state['values']['practical_experience'] as $i => $value) {
            create_practical_experience($form, $i, array(''));
        }

        if ($form_state['clicked_button']['#name'] == 'practical_experience_add') {
            // Add the additional fields for a new entry.
            $last = count($form_state['values']['practical_experience']);
            create_practical_experience($form, $last, array(''));
        }
    }

    $form['practical_experience']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'practical_experience_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_practical_experience',
            'wrapper' => 'replace-practical-experience',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    $form['save_button'] = array(
        '#type' => 'submit',
        '#value' => t('Speichern'),
        '#submit' => array('save_button_submit'),
        '#prefix' => '<br>',
    );

    return $form;

}

function create_practical_experience(&$form, $row, $defaultValues) {

    $form['practical_experience'][$row]['cv_wrapper_start'] = array(
        '#markup' => '<fieldset><div class="fieldset-wrapper">'
    );

    $form['practical_experience'][$row]['activity'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Tätigkeit'),
        '#default_value' => (isset($defaultValues['activity'])) ? $defaultValues['activity'] : '',

    );

    $form['practical_experience'][$row]['employment'] = array(
        '#type' => 'select', //you can find a list of available types in the form api
        '#title' => t('Beschäftigungsart'),
        '#options' => array(
            t('- auswählen -') => t('- auswählen -'),
            t('Ausbildung') => t('Ausbildung'),
            t('Praktikum') => t('Praktikum'),
            t('Aushilfsjob') => t('Aushilfsjob'),
            t('Teilzeit') => t('Teilzeit'),
            t('Vollzeit') => t('Vollzeit'),
            t('Werksstudent') => t('Werksstudent'),
        ),
        '#default_value' => (isset($defaultValues['employment'])) ? $defaultValues['employment'] : '',
    );

    $form['practical_experience'][$row]['company'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Unternehmen'),
        '#default_value' => (isset($defaultValues['company'])) ? $defaultValues['company'] : '',

    );

    $form['practical_experience'][$row]['sector'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Branche'),
        '#default_value' => (isset($defaultValues['sector'])) ? $defaultValues['sector'] : '',

    );

    $form['practical_experience'][$row]['cv_wrapper_end'] = array(
        '#markup' => '</div></fieldset>'
    );
}

function callback_practical_experience($form, &$form_state) {
    return $form['practical_experience'];
}

function delete_image_button_submit($form, &$form_state) {
    $file = file_load($form_state['values']['profile_image_id']);

    file_delete($file);

    db_update('extended_user_profile')
        ->fields(array(
            'image_id' => 0,
        ))
        ->condition('user_id', $form_state['values']['profile_user_id'], '=')
        ->execute();
}

function save_button_submit($form, &$form_state) {
    dvm($form_state['values']);
}