<?php

function display_usercv ($user_id) {
    global $user;
    $content = array();

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

    $result = db_select('extended_user_profile_cv', 'ex')
        ->fields('ex', $fields)
        ->condition('user_id', $user_id, '=')
        ->execute()
        ->fetchAssoc();

    dvm($result);


    $content['cv_container_start'] = array(
        '#markup' => '<div class="cv-container">',
    );

    $imageHTML = '';
    if ($result['image_id'] != 0) {
        $image = file_load($result['image_id']);


        $variables = array(
            'path' => $image->uri,
            'alt' => t('Profilbild'),
            'title' => t('Profilbild'),
            'width' => '180px',
        );
        $imageHTML = theme('image', $variables);

    }

    $content['picture_container'] = array(
        '#markup' => '<div class="cv-picture-container">' . $imageHTML . '</div>'
    );

    $content['cv_container_end'] = array(
        '#markup' => '</div>',
    );

    if ($user_id == $user->uid || user_has_role(3, user_load($user->uid))) {
        $content['edit_button'] = array(
            '#markup' => '<br><a class="btn" href="' . base_path() . 'user/' . $user_id . '/usercv/edit">' . t('Editieren') . '</a>',
        );
    }

    return $content;
}