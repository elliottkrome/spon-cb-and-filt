;;; Directory Local Variables
;;; For more information see (info "(emacs) Directory Variables")
((nil . ((eval . (when buffer-file-name
		   (setq org-download-image-dir
			 (concat (locate-dominating-file ;; project root
				  buffer-file-name
				  ".dir-locals.el")
				 "resources/images" ;; sub-directory
				 )))))))
