<?php

function extended_user_profile_form($form, &$form_state, $user_id) {

    $userLoad = user_load($user_id);
    if(!$userLoad){
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
        'first_name',
        'last_name',
        'jufo_projects',
        'other_projects',
        'visited_schools',
        'city',
        'zip_code',
        'telephone',
        'other_profiles',
        'check_email',
        'check_post',
        'check_telephone',
        'birthday_day',
        'birthday_month',
        'birthday_year',
        'gender',
        'hobbies',
        'interests',
        'received_awards',
        'organisations'
    );

    $user_id = ($user_id != $user->uid && user_has_role(3, user_load($user->uid))) ? $user_id : $user->uid;
    $result = db_select('extended_user_profile', 'ex')
        ->fields('ex', $fields)
        ->condition('user_id', $user_id, '=')
        ->execute()
        ->fetchAssoc();

    $form['profile_user_id'] = array(
        '#type' => 'hidden',
        '#value' => $user_id,
    );

    $tabsHTML =
        '<div class="tab">' .
        '<a class="tablinks active" target="general">' . t('Allgemein') . '</a>' .
        '<a class="tablinks" target="contact-preference">' . t('Kontaktpräferenz') . '</a>' .
        '<a class="tablinks" target="personal">' . t('Persönlich') . '</a>' .
        '<a class="tablinks" target="others">' . t('Sonstiges') . '</a>' .
        '</div>';

    $form['tabarea'] = array(
        '#markup' => $tabsHTML,
    );

    $form['general_start'] = array(
        '#markup' => '<div id="general" class="tabcontent">',
    );


    //Nutzerprofil Allgemein

    $form['general_headline'] = array(
        '#markup' => '<h3>' . t('Allgemein') . '</h3>',
    );

    if ($result['image_id'] != 0) {

        $image = file_load($result['image_id']);

        $form['profile_image'] = array(
            '#markup' => '<img src="' . file_create_url($image->uri) . '" width="200px"><br>'
        );

        $form['delete_image'] = array(
            '#type' => 'submit',
            '#value' => t('Bild löschen'),
            '#submit' => array('delete_image_button_submit'),
        );

        $form['profile_image_id'] = array(
            '#type' => 'hidden',
            '#value' => $result['image_id'],
        );
    }

    $form['profile_image_upload'] = array(
        '#type' => 'file',
        '#title' => t('Profilbild hochladen'),
        '#description' => t('Erlaubte Erweiterungen: jpg, jpeg, png, gif'),
    );


    $form['firstname'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Vorname'),
        '#prefix' => '<br><hr><br>',
        '#default_value' => (isset($result['first_name']) && !empty($result['first_name'])) ? $result['first_name'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>',
    );

    $form['lastname'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Nachname'),
        '#default_value' => (isset($result['last_name']) && !empty($result['last_name'])) ? $result['last_name'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>',
    );

    $form['jf_projects'] = array(
        '#type' => 'fieldset',
        '#title' => t('Jugend Forscht Projekt auswählen'),
        '#prefix' => '<div id="replace-jf-projects">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    // Build the initial fields if this is not being rebuilt
    // from ajax request.
    if (!array_key_exists('clicked_button', $form_state)) {

        if (isset($result['jufo_projects'])) {
            $jfProjects = unserialize($result['jufo_projects']);

            if (count($jfProjects) != 0) {
                for ($i = 0; $i < count($jfProjects); $i++) {
                    create_jf_project($form, $i, t($jfProjects[$i]));

                }
            }else {
                create_jf_project($form, 0, '');
            }
        } else {
            create_jf_project($form, 0, '');
        }
    } else {


        // Otherwise add the fields for each existing value.
        foreach ($form_state['values']['jf_projects'] as $i => $value) {
            create_jf_project($form, $i, '');
        }

        if ($form_state['clicked_button']['#name'] == 'jf_projects_add') {
            // Add the additional fields for a new entry.
            $last = count($form_state['values']['jf_projects']);
            create_jf_project($form, $last, '');
        }
    }

    $form['jf_projects']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'jf_projects_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_jf_projects',
            'wrapper' => 'replace-jf-projects',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    $form['jf_projects']['jf_project_db'] = array(
        '#type' => 'button',
        '#value' => t('JuFo-Projektdatenbank'),
        '#weight' => 1001
    );

    $form['jf_projects']['suggest_project'] = array(
        '#type' => 'button',
        '#value' => t('Projekt vorschlagen'),
        '#weight' => 1002
    );


    $form['other_projects'] = array(
        '#type' => 'fieldset',
        '#title' => t('Weitere Projekte auswählen'),
        '#prefix' => '<div id="replace-other-projects">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    // Build the initial fields if this is not being rebuilt
    // from ajax request.
    if (!array_key_exists('clicked_button', $form_state)) {
        if (isset($result['other_projects'])) {
            $otherProjects = unserialize($result['other_projects']);

            if (count($otherProjects) != 0) {
                for ($i = 0; $i < count($otherProjects); $i++) {
                    create_other_project($form, $i, t($otherProjects[$i]));
                }
            } else {
                create_other_project($form, 0, '');
            }
        }else {
            create_other_project($form, 0, '');
        }
    } else {


        // Otherwise add the fields for each existing value.
        foreach ($form_state['values']['other_projects'] as $i => $value) {
            create_other_project($form, $i, '');
        }

        if ($form_state['clicked_button']['#name'] == 'other_projects_add') {
            // Add the additional fields for a new entry.
            $last = count($form_state['values']['other_projects']);
            create_other_project($form, $last, '');
        }
    }

    $form['other_projects']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'other_projects_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_other_projects',
            'wrapper' => 'replace-other-projects',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );


    $form['visited_schools'] = array(
        '#type' => 'fieldset',
        '#title' => t('Besuchte Schule auswählen'),
        '#prefix' => '<div id="replace-visited-schools">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    // Build the initial fields if this is not being rebuilt
    // from ajax request.
    if (!array_key_exists('clicked_button', $form_state)) {
        if (isset($result['visited_schools'])) {
            $visitedSchools = unserialize($result['visited_schools']);

            if (count($visitedSchools) != 0) {
                for ($i = 0; $i < count($visitedSchools); $i++) {
                    create_visited_school($form, $i, t($visitedSchools[$i]));
                }
            } else {
                create_visited_school($form, 0, '');
            }
        } else {
            create_visited_school($form, 0, '');
        }
    } else {


        // Otherwise add the fields for each existing value.
        foreach ($form_state['values']['visited_schools'] as $i => $value) {
            create_visited_school($form, $i, '');
        }

        if ($form_state['clicked_button']['#name'] == 'visited_schools_add') {
            // Add the additional fields for a new entry.
            $last = count($form_state['values']['visited_schools']);
            create_visited_school($form, $last, '');
        }
    }

    $form['visited_schools']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'visited_schools_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_visited_schools',
            'wrapper' => 'replace-visited-schools',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    $form['visited_schools']['suggest_school'] = array(
        '#type' => 'button',
        '#value' => t('Schule vorschlagen'),
        '#weight' => 1001
    );

    //Nutzerprofil Kontaktdaten

    $form['contactdetails_headline'] = array(
        '#markup' => '<br><hr><br><h3>' . t('Kontaktdaten') . '</h3>',
    );

    $form['city'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Wohnort'),
        '#default_value' => (isset($result['city']) && !empty($result['city'])) ? $result['city'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>',
    );

    $form['zip_code'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('PLZ'),
        '#default_value' => (isset($result['zip_code']) && $result['zip_code'] != 0) ? $result['zip_code'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>',
    );

    $form['telephone'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Telefon/Mobil'),
        '#default_value' => (isset($result['telephone']) && !empty($result['telephone'])) ? $result['telephone'] : '',
    );

    $form['other_profiles'] = array(
        '#type' => 'fieldset',
        '#title' => t('Weitere Profile auswählen'),
        '#prefix' => '<div id="replace-other-profiles">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    // Build the initial fields if this is not being rebuilt
    // from ajax request.
    if (!array_key_exists('clicked_button', $form_state)) {
        if (isset($result['other_profiles'])) {
            $otherProfiles = unserialize($result['other_profiles']);
            if (count($otherProfiles) != 0) {
                $index = 0;

                for ($i = 0; $i < count($otherProfiles); $i++) {
                //foreach ($otherProfiles as $key => $value) {
                    create_other_profile($form, $i, $otherProfiles[$i]);
                    $index++;
                }

            } else {
                create_other_profile($form, 0, null);
            }

        } else {
            create_other_profile($form, 0, null);
        }
    } else {


        // Otherwise add the fields for each existing value.
        foreach ($form_state['values']['other_profiles'] as $i => $value) {
            create_other_profile($form, $i, null);
        }

        if ($form_state['clicked_button']['#name'] == 'other_profiles_add') {
            // Add the additional fields for a new entry.
            $last = count($form_state['values']['other_profiles']);
            create_other_profile($form, $last, null);
        }
    }

    $form['other_profiles']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'other_profiles_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_other_profiles',
            'wrapper' => 'replace-other-profiles',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    $form['general_end'] = array(
        '#markup' => '</div>',
    );

    //Nutzerprofil Kontaktpräferenz

    $form['contactpreference_start'] = array(
        '#markup' => '<div id="contact-preference" class="tabcontent">',
    );

    $form['contactpreference_headline'] = array(
        '#markup' => t('Über folgende Kanäle darf Kontakt zu mir aufgenommen werden'),
    );

    $form['check_email'] = array(
        '#type' => 'checkbox',
        '#title' => t('E-Mail'),
        '#default_value' => (isset($result['check_email'])) ? $result['check_email'] : 0,
    );

    $form['check_post'] = array(
        '#type' => 'checkbox',
        '#title' => t('Post'),
        '#default_value' => (isset($result['check_post'])) ? $result['check_post'] : 0,
    );

    $form['check_telephone'] = array(
        '#type' => 'checkbox',
        '#title' => t('Telefon/Mobil'),
        '#default_value' => (isset($result['check_telephone'])) ? $result['check_telephone'] : 0,
    );

    $form['contactpreference_end'] = array(
        '#markup' => '</div>',
    );

    //Nutzerprofil Persönlich
    $form['personal_start'] = array(
        '#markup' => '<div id="personal" class="tabcontent">',
    );

    $days = array(SELECT_DEFAULT => SELECT_DEFAULT);
    for ($i = 1; $i <= 31; $i++) {
        $days[strval($i)] = strval($i);
    }

    $months = array(
        SELECT_DEFAULT => SELECT_DEFAULT,
        t('Januar') => t('Januar'),
        t('Februar') => t('Februar'),
        t('März') => t('März'),
        t('April') => t('April'),
        t('Mai') => t('Mai'),
        t('Juni') => t('Juni'),
        t('Juli') => t('Juli'),
        t('August') => t('August'),
        t('September') => t('September'),
        t('Oktober') => t('Oktober'),
        t('November') => t('November'),
        t('Dezember') => t('Dezember'),
    );

    $years = array(SELECT_DEFAULT => SELECT_DEFAULT);

    for ($i = date("Y"); $i >= 1950; $i--) {
        $years[strval($i)] = strval($i);
    }

    $form['birthday'] = array(
        '#markup' => '<b>' . t('Geburtstag') . ':</b><br>',
    );

    $form['birthday_day'] = array(
        '#type' => 'select', //you can find a list of available types in the form api
        '#title' => t('Tag'),
        '#options' => $days,
        '#default_value' => (isset($result['birthday_day']) && $result['birthday_day'] != 0) ? $result['birthday_day'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>'
    );

    $form['birthday_month'] = array(
        '#type' => 'select', //you can find a list of available types in the form api
        '#title' => t('Monat'),
        '#options' => $months,
        '#default_value' => (isset($result['birthday_month']) && !empty($result['birthday_month'])) ? $result['birthday_month'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>'
    );

    $form['birthday_year'] = array(
        '#type' => 'select', //you can find a list of available types in the form api
        '#title' => t('Jahr'),
        '#options' => $years,
        '#default_value' => (isset($result['birthday_year']) && $result['birthday_year'] != 0) ? $result['birthday_year'] : SELECT_DEFAULT,
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>'
    );

    $active = array(t('männlich') => t('männlich'), t('weiblich') => t('weiblich'));
    $form['gender'] = array(
        '#type' => 'radios',
        '#title' => t('Geschlecht'),
        '#options' => $active,
        '#default_value' => (isset($result['gender']) && $result['gender'] != null) ? $result['gender'] : '',
    );


    $form['hobbies'] = array(
        '#type' => 'fieldset',
        '#title' => t('Hobbys'),
        '#prefix' => '<div id="replace-hobbies">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    // Build the initial fields if this is not being rebuilt
    // from ajax request.
    if (!array_key_exists('clicked_button', $form_state)) {
        if (isset($result['hobbies'])) {
            $hobbies = unserialize($result['hobbies']);

            if (count($hobbies) != 0) {
                for ($i = 0; $i < count($hobbies); $i++) {
                    create_hobbie($form, $i, t($hobbies[$i]));
                }
            } else {
                create_hobbie($form, 0, '');
            }
        } else {
            create_hobbie($form, 0, '');
        }
    } else {
        // Otherwise add the fields for each existing value.
        foreach ($form_state['values']['hobbies'] as $i => $value) {
            create_hobbie($form, $i, '');
        }

        if ($form_state['clicked_button']['#name'] == 'hobbies_add') {
            // Add the additional fields for a new entry.
            $last = count($form_state['values']['hobbies']);
            create_hobbie($form, $last, '');
        }
    }

    $form['hobbies']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'hobbies_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_hobbies',
            'wrapper' => 'replace-hobbies',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    $form['interests'] = array(
        '#type' => 'fieldset',
        '#title' => t('Interessen'),
        '#prefix' => '<div id="replace-interests">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    // Build the initial fields if this is not being rebuilt
    // from ajax request.
    if (!array_key_exists('clicked_button', $form_state)) {
        if (isset($result['interests'])) {
            $interests = unserialize($result['interests']);

            if (count($interests) != 0) {
                for ($i = 0; $i < count($interests); $i++) {
                    create_interest($form, $i, t($interests[$i]));
                }
            } else {
                create_interest($form, 0, '');
            }
        } else {
            create_interest($form, 0, '');
        }
    } else {
        // Otherwise add the fields for each existing value.
        foreach ($form_state['values']['interests'] as $i => $value) {
            create_interest($form, $i, '');
        }

        if ($form_state['clicked_button']['#name'] == 'interests_add') {
            // Add the additional fields for a new entry.
            $last = count($form_state['values']['interests']);
            create_interest($form, $last, '');
        }
    }

    $form['interests']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'interests_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_interests',
            'wrapper' => 'replace-interests',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    $form['personal_end'] = array(
        '#markup' => '</div>',
    );

    //Nutzerprofil Others
    $form['others_start'] = array(
        '#markup' => '<div id="others" class="tabcontent">',
    );

    $form['awards'] = array(
        '#type' => 'fieldset',
        '#title' => t('Erhaltene Auszeichnungen'),
        '#prefix' => '<div id="replace-awards">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    // Build the initial fields if this is not being rebuilt
    // from ajax request.
    if (!array_key_exists('clicked_button', $form_state)) {
        if (isset($result['received_awards'])) {
            $awards = unserialize($result['received_awards']);

            if (count($awards) != 0) {
                for ($i = 0; $i < count($awards); $i++) {
                    create_award($form, $i, t($awards[$i]));
                }
            } else {
                create_award($form, 0, '');
            }
        } else {
            create_award($form, 0, '');
        }
    } else {
        // Otherwise add the fields for each existing value.
        foreach ($form_state['values']['awards'] as $i => $value) {
            create_award($form, $i, '');
        }

        if ($form_state['clicked_button']['#name'] == 'awards_add') {
            // Add the additional fields for a new entry.
            $last = count($form_state['values']['awards']);
            create_award($form, $last, '');
        }
    }

    $form['awards']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'awards_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_awards',
            'wrapper' => 'replace-awards',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    $form['organisations'] = array(
        '#type' => 'fieldset',
        '#title' => t('Organisationen'),
        '#prefix' => '<div id="replace-organisations">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    // Build the initial fields if this is not being rebuilt
    // from ajax request.
    if (!array_key_exists('clicked_button', $form_state)) {
        if (isset($result['organisations'])) {
            $organisations = unserialize($result['organisations']);

            if (count($organisations) != 0) {
                for ($i = 0; $i < count($organisations); $i++) {
                    create_organisation($form, $i, t($organisations[$i]));
                }
            } else {
                create_organisation($form, 0, '');
            }
        } else {
            create_organisation($form, 0, '');
        }
    } else {
        // Otherwise add the fields for each existing value.
        foreach ($form_state['values']['organisations'] as $i => $value) {
            create_organisation($form, $i, '');
        }

        if ($form_state['clicked_button']['#name'] == 'organisations_add') {
            // Add the additional fields for a new entry.
            $last = count($form_state['values']['organisations']);
            create_organisation($form, $last, '');
        }
    }

    $form['organisations']['add_more'] = array(
        '#type' => 'button',
        '#name' => 'organisations_add',
        '#value' => t('Mehr'),
        '#ajax' => array(
            'callback' => 'callback_organisations',
            'wrapper' => 'replace-organisations',
            'method' => 'replace',
        ),
        '#weight' => 1000
    );

    $form['others_end'] = array(
        '#markup' => '</div>',
    );


    $form['save_button'] = array(
        '#type' => 'submit',
        '#value' => t('Speichern'),
        '#submit' => array('save_button_submit'),
        '#prefix' => '<br>',
    );

    return $form;

}



function create_jf_project(&$form, $row, $defaultValue) {
    //$form['agencies'][$row] = array('#type' => 'fieldset');

    $form['jf_projects'][$row]['jf_projects_select'] = array(
        '#type' => 'select', //you can find a list of available types in the form api
        '#options' => array(
            SELECT_DEFAULT => SELECT_DEFAULT,
            t('Projekt 1') => t('Projekt 1'),
            t('Projekt 2') => t('Projekt 2'),
            t('Projekt 3') => t('Projekt 3'),
        ),
        '#default_value' => $defaultValue,
    );

}

function create_other_project(&$form, $row, $defaultValue) {
    $form['other_projects'][$row]['other_projects_input'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#default_value' => $defaultValue,
    );

}

function create_visited_school(&$form, $row, $defaultValue) {

    $form['visited_schools'][$row]['visited_schools_select'] = array(
        '#type' => 'select', //you can find a list of available types in the form api
        '#options' => array(
            SELECT_DEFAULT => SELECT_DEFAULT,
            t('Schule A') => t('Schule A'),
            t('Schule B') => t('Schule B'),
            t('Schule C') => t('Schule C'),
        ),
        '#default_value' => $defaultValue,
    );
}

function create_other_profile(&$form, $row, $defaultValues) {

    $form['other_profiles'][$row]['wrapper_start'] = array(
        '#markup' => '<fieldset><div class="fieldset-wrapper">'
    );
    $form['other_profiles'][$row]['choose_portal'] = array(
        '#type' => 'select', //you can find a list of available types in the form api
        '#title' => t('Portal'),
        '#options' => array(
            SELECT_DEFAULT => SELECT_DEFAULT,
            t('Facebook') => t('Facebook'),
            t('Google+') => t('Google+'),
            t('Instagram') => t('Instagram'),
            t('LinkedIn') => t('LinkedIn'),
            t('Pinterest') => t('Pinterest'),
            t('Snapchat') => t('Snapchat'),
            t('Tumblr') => t('Tumblr'),
            t('Twitter') => t('Twitter'),
            t('XING') => t('XING'),
        ),
        '#default_value' => (isset($defaultValues['choose_portal']) && !empty($defaultValues['choose_portal'])) ? $defaultValues['choose_portal'] : '',
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div>',
    );

    $form['other_profiles'][$row]['portal_link'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#title' => t('Link zum Profil'),
        '#default_value' => $defaultValues['portal_link'],
        '#prefix' => '<div class="left-floated-element">',
        '#suffix' => '</div><div class="clear-float"></div>',
    );

    $form['other_profiles'][$row]['wrapper_end'] = array(
        '#markup' => '</div></fieldset>'
    );
}

function create_hobbie(&$form, $row, $defaultValue) {

    $form['hobbies'][$row]['hobbies_input'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#default_value' => $defaultValue,
    );
}

function create_interest(&$form, $row, $defaultValue) {

    $form['interests'][$row]['interests_select'] = array(
        '#type' => 'select', //you can find a list of available types in the form api
        '#options' => array(
            SELECT_DEFAULT => SELECT_DEFAULT,
            t('Betriebswirtschaft') => t('Betriebswirtschaft'),
            t('Humanwissenschaften') => t('Humanwissenschaften'),
            t('Informatik') => t('Informatik'),
            t('Kulturwissenschaften') => t('Kulturwissenschaften'),
            t('Naturwissenschaften') => t('Naturwissenschaften'),
            t('Philosophie') => t('Philosophie'),
            t('Physik') => t('Physik'),
            t('Politikwissenschaften') => t('Politikwissenschaften'),
            t('Psychologie') => t('Psychologie'),
        ),
        '#default_value' => $defaultValue,
    );

}

function create_award(&$form, $row, $defaultValue) {

    $form['awards'][$row]['awards_input'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#default_value' => $defaultValue,
    );

    $form['awards'][$row]['awards_input']['#attributes']['placeholder'] = t('z.B. eine Auszeichnung für "Jugend Forscht" Projekte erhalten');
}

function create_organisation(&$form, $row, $defaultValue) {

    $form['organisations'][$row]['organisations_input'] = array(
        '#type' => 'textfield', //you can find a list of available types in the form api
        '#default_value' => $defaultValue,
    );

    $form['organisations'][$row]['organisations_input']['#attributes']['placeholder'] = t('z.B. Engagement bei der freiwilligen Feuerwehr');
}


function callback_jf_projects($form, &$form_state) {
    return $form['jf_projects'];
}

function callback_other_projects($form, &$form_state) {
    return $form['other_projects'];
}

function callback_visited_schools($form, &$form_state) {
    return $form['visited_schools'];
}

function callback_other_profiles($form, &$form_state) {
    return $form['other_profiles'];
}

function callback_hobbies($form, &$form_state) {
    return $form['hobbies'];
}

function callback_interests($form, &$form_state) {
    return $form['interests'];
}

function callback_awards($form, &$form_state) {
    return $form['awards'];
}

function callback_organisations($form, &$form_state) {
    return $form['organisations'];
}


function extended_user_profile_form_validate($form, &$form_state) {
    if (!empty($form_state['values']['zip_code'])) {
        if (!preg_match('/^[0-9]+$/', $form_state['values']['zip_code'])) {
            form_set_error('zip_code', t('Postleitzahl darf nur Zahlen enthalten.'));
        }
    }

    $index = 0;
    foreach ($form_state['values']['other_profiles'] as $value) {
        if (isset($value['portal_link'])) {
            if (!preg_match('/(http|https):\/\//', $value['portal_link']) && !empty($value['portal_link'])) {
                form_set_error('other_profiles][' . $index . '][portal_link', t('Kein gültiger Link beim Portal angegeben.'));
            }
        }
        $index++;
    }

    $file = file_save_upload('profile_image_upload', array(
        'file_validate_is_image' => array(),
        'file_validate_extensions' => array('png gif jpg jpeg'),
    ));

    if ($file != null) {
        if ($file) {
            if ($file = file_move($file, 'public://profile_images')) {
                $file->status = FILE_STATUS_PERMANENT;

                file_save($file);
                $form_state['values']['profile_image_upload'] = $file;
            } else {
                form_set_error('file', t('Bild konnte nicht geschrieben werden.'));
            }
        } else {
            form_set_error('file', t('Kein Bild wurde hochgeladen.'));
        }
    }
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

    $jfProjects = array();

    foreach ($form_state['values']['jf_projects'] as $value) {
        if (isset($value['jf_projects_select'])) {
            if ($value['jf_projects_select'] != SELECT_DEFAULT) {
                array_push($jfProjects, $value['jf_projects_select']);

            }
        }
    }

    $otherProjects = array();

    foreach ($form_state['values']['other_projects'] as $value) {
        if (isset($value['other_projects_input'])) {
            if (!empty($value['other_projects_input'])) {
                array_push($otherProjects, $value['other_projects_input']);

            }
        }
    }

    $visitedSchools = array();

    foreach ($form_state['values']['visited_schools'] as $value) {
        if (isset($value['visited_schools_select'])) {
            if ($value['visited_schools_select'] != SELECT_DEFAULT) {
                array_push($visitedSchools, $value['visited_schools_select']);

            }
        }
    }

    $otherProfiles = array();

    foreach ($form_state['values']['other_profiles'] as $value) {
        if (isset($value['choose_portal'])) {
            if ($value['choose_portal'] != SELECT_DEFAULT && !empty($value['portal_link'])) {
                array_push($otherProfiles, array(
                    'choose_portal' => $value['choose_portal'],
                    'portal_link' => $value['portal_link'],
                ));
            }
        }
    }


    $hobbies = array();

    foreach ($form_state['values']['hobbies'] as $value) {
        if (isset($value['hobbies_input'])) {
            if (!empty($value['hobbies_input'])) {
                array_push($hobbies, $value['hobbies_input']);

            }
        }
    }

    $interests = array();

    foreach ($form_state['values']['interests'] as $value) {
        if (isset($value['interests_select'])) {
            if ($value['interests_select'] != SELECT_DEFAULT) {
                array_push($interests, $value['interests_select']);

            }
        }
    }

    $awards = array();

    foreach ($form_state['values']['awards'] as $value) {
        if (isset($value['awards_input'])) {
            if (!empty($value['awards_input'])) {
                array_push($awards, $value['awards_input']);

            }
        }
    }

    $organisations = array();

    foreach ($form_state['values']['organisations'] as $value) {
        if (isset($value['organisations_input'])) {
            if (!empty($value['organisations_input'])) {
                array_push($organisations, $value['organisations_input']);

            }
        }
    }

    $imageID = 0;

    if (isset($form_state['values']['profile_image_id'])) {
        $imageID = $form_state['values']['profile_image_id'];
    }

    if (isset($form_state['values']['profile_image_upload']->fid)) {
        $imageID = $form_state['values']['profile_image_upload']->fid;

        if (isset($form_state['values']['profile_image_id'])) {
            $file = file_load($form_state['values']['profile_image_id']);

            file_delete($file);
        }
    }

    $result = db_select('extended_user_profile', 'ex')
        ->fields('ex', array('user_id'))
        ->condition('user_id', $form_state['values']['profile_user_id'], '=')
        ->execute();

    if (!$result->fetchAssoc()) {

        // Here u can insert Your custom form values into your custom table.
        db_insert('extended_user_profile')
            ->fields(array(
                'user_id' => $form_state['values']['profile_user_id'],
                'image_id' => $imageID,
                'first_name' => $form_state['values']['firstname'],
                'last_name' => $form_state['values']['lastname'],
                'jufo_projects' => serialize($jfProjects),
                'other_projects' => serialize($otherProjects),
                'visited_schools' => serialize($visitedSchools),
                'city' => $form_state['values']['city'],
                'zip_code' => !empty($form_state['values']['zip_code']) ? $form_state['values']['zip_code'] : 0,
                'telephone' => $form_state['values']['telephone'],
                'other_profiles' => serialize($otherProfiles),
                'check_email' => !empty($form_state['values']['check_email']) ? $form_state['values']['check_email'] : 0,
                'check_post' => !empty($form_state['values']['check_post']) ? $form_state['values']['check_post'] : 0,
                'check_telephone' => !empty($form_state['values']['check_telephone']) ? $form_state['values']['check_telephone'] : 0,
                'birthday_day' => ($form_state['values']['birthday_day'] != SELECT_DEFAULT) ? $form_state['values']['birthday_day'] : 0,
                'birthday_month' => ($form_state['values']['birthday_month'] != SELECT_DEFAULT) ? $form_state['values']['birthday_month'] : '',
                'birthday_year' => ($form_state['values']['birthday_year'] != SELECT_DEFAULT) ? $form_state['values']['birthday_year'] : 0,
                'gender' => $form_state['values']['gender'],
                'hobbies' => serialize($hobbies),
                'interests' => serialize($interests),
                'received_awards' => serialize($awards),
                'organisations' => serialize($organisations),
            ))
            ->execute();
        drupal_set_message(t('Informationen wurden gespeichert.'));

    } else {
        db_update('extended_user_profile')
            ->fields(array(
                'image_id' => $imageID,
                'first_name' => $form_state['values']['firstname'],
                'last_name' => $form_state['values']['lastname'],
                'jufo_projects' => serialize($jfProjects),
                'other_projects' => serialize($otherProjects),
                'visited_schools' => serialize($visitedSchools),
                'city' => $form_state['values']['city'],
                'zip_code' => !empty($form_state['values']['zip_code']) ? $form_state['values']['zip_code'] : 0,
                'telephone' => $form_state['values']['telephone'],
                'other_profiles' => serialize($otherProfiles),
                'check_email' => !empty($form_state['values']['check_email']) ? $form_state['values']['check_email'] : 0,
                'check_post' => !empty($form_state['values']['check_post']) ? $form_state['values']['check_post'] : 0,
                'check_telephone' => !empty($form_state['values']['check_telephone']) ? $form_state['values']['check_telephone'] : 0,
                'birthday_day' => ($form_state['values']['birthday_day'] != SELECT_DEFAULT) ? $form_state['values']['birthday_day'] : 0,
                'birthday_month' => ($form_state['values']['birthday_month'] != SELECT_DEFAULT) ? $form_state['values']['birthday_month'] : '',
                'birthday_year' => ($form_state['values']['birthday_year'] != SELECT_DEFAULT) ? $form_state['values']['birthday_year'] : 0,
                'gender' => $form_state['values']['gender'],
                'hobbies' => serialize($hobbies),
                'interests' => serialize($interests),
                'received_awards' => serialize($awards),
                'organisations' => serialize($organisations),
            ))
            ->condition('user_id', $form_state['values']['profile_user_id'], '=')
            ->execute();

        drupal_set_message(t('Informationen wurden geändert.'));
    }

}
