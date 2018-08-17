;;; Directory Local Variables
;;; For more information see (info "(emacs) Directory Variables")
((nil . ((eval . (when buffer-file-name
		   (progn
		     (setq spon-filt-base-dir (locate-dominating-file 
					       buffer-file-name
					       ".dir-locals.el"))
		     (setq org-download-image-dir
			   (concat spon-filt-base-dir
				   "resources/images"))
		     ))))))
